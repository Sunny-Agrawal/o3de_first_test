// SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
// SPDX-License-Identifier: LicenseRef-NvidiaProprietary
//
// NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
// property and proprietary rights in and to this material, related
// documentation and any modifications thereto. Any use, reproduction,
// disclosure or distribution of this material and related documentation
// without an express license agreement from NVIDIA CORPORATION or
// its affiliates is strictly prohibited.

#pragma once

#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/array.h>
#include <pxr/usd/usdGeom/tokens.h>

#include <map>

namespace usdex::core
{

template <typename T>
PrimvarData<T>::PrimvarData(const pxr::TfToken& interpolation, const pxr::VtArray<T>& values, int elementSize)
    : m_interpolation(interpolation), m_elementSize(elementSize), m_values(values)
{
}

template <typename T>
PrimvarData<T>::PrimvarData(const pxr::TfToken& interpolation, const pxr::VtArray<T>& values, const pxr::VtArray<int>& indices, int elementSize)
    : m_interpolation(interpolation), m_elementSize(elementSize), m_values(values), m_indices(indices)
{
}

template <typename T>
PrimvarData<T> PrimvarData<T>::getPrimvarData(const pxr::UsdGeomPrimvar& primvar, pxr::UsdTimeCode time)
{
    if (!primvar)
    {
        return PrimvarData<T>(pxr::UsdGeomTokens->constant, {}, -1);
    }

    int elementSize = primvar.HasAuthoredElementSize() ? primvar.GetElementSize() : -1;

    pxr::VtArray<T> values;
    if (!primvar.Get<pxr::VtArray<T>>(&values, time))
    {
        return PrimvarData<T>(pxr::UsdGeomTokens->constant, {}, -1);
    }

    if (primvar.IsIndexed())
    {
        pxr::VtIntArray indices;
        primvar.GetIndices(&indices, time);
        return PrimvarData<T>(primvar.GetInterpolation(), values, indices, elementSize);
    }
    else
    {
        return PrimvarData<T>(primvar.GetInterpolation(), values, elementSize);
    }
}

template <typename T>
bool PrimvarData<T>::setPrimvar(pxr::UsdGeomPrimvar& primvar, pxr::UsdTimeCode time) const
{
    if (!primvar)
    {
        return false;
    }

    if (!primvar.SetInterpolation(m_interpolation))
    {
        return false;
    }

    if (!primvar.Set(m_values, time))
    {
        return false;
    }

    // Author an explicit opinion about the indices to ensure weaker opinions are overriden
    if (hasIndices())
    {
        if (!primvar.SetIndices(m_indices, time))
        {
            return false;
        }
    }
    else
    {
        primvar.BlockIndices();
    }

    if (m_elementSize > 0)
    {
        primvar.SetElementSize(m_elementSize);
    }
    else if (primvar.HasAuthoredElementSize())
    {
        // if the elementSize was previously authored, we need to reset it as there is no way to block element size
        primvar.SetElementSize(1);
    }

    return true;
}

template <typename T>
const pxr::TfToken& PrimvarData<T>::interpolation() const
{
    return m_interpolation;
}

template <typename T>
const pxr::VtArray<T>& PrimvarData<T>::values() const
{
    return m_values;
}

template <typename T>
bool PrimvarData<T>::hasIndices() const
{
    return !m_indices.empty();
}

template <typename T>
const pxr::VtArray<int>& PrimvarData<T>::indices() const
{
    if (!m_indices.empty())
    {
        return m_indices;
    }

    throw std::runtime_error("It is invalid to call indices() on PrimvarData unless hasIndices() returns true");
}

template <typename T>
int PrimvarData<T>::elementSize() const
{
    return m_elementSize;
}

template <typename T>
size_t PrimvarData<T>::effectiveSize() const
{
    if (m_elementSize > 0)
    {
        return m_indices.empty() ? m_values.size() / m_elementSize : m_indices.size() / m_elementSize;
    }
    else
    {
        return m_indices.empty() ? m_values.size() : m_indices.size();
    }
}

template <typename T>
bool PrimvarData<T>::isValid() const
{
    if (!pxr::UsdGeomPrimvar::IsValidInterpolation(m_interpolation))
    {
        return false;
    }

    if (m_values.empty())
    {
        return false;
    }

    if (m_indices.empty())
    {
        if (m_elementSize > 0 && (m_values.size() % m_elementSize))
        {
            return false;
        }
    }
    else
    {
        if (m_elementSize > 0 && (m_indices.size() % m_elementSize))
        {
            return false;
        }

        size_t maxIndex = m_values.size() - 1;
        for (const int i : indices())
        {
            if (i < 0 || (size_t)i > maxIndex)
            {
                return false;
            }
        }
    }

    return true;
}

template <typename T>
bool PrimvarData<T>::isIdentical(const PrimvarData& other) const
{
    return (
        (m_interpolation == other.interpolation()) && (m_elementSize == other.elementSize()) && (this->hasIndices() == other.hasIndices()) &&
        m_values.IsIdentical(other.values()) && m_indices.IsIdentical(other.indices())
    );
}

template <typename T>
bool PrimvarData<T>::index()
{
    // Abort indexing if the element size is greater than one
    // We do not fully understand the correct manner by which indexing should be described when element size is involved.
    if (m_elementSize > 1)
    {
        // this is a TF_RUNTIME_ERROR, but we have expanded the code manually to inject the class namespaces
        pxr::Tf_PostErrorHelper(
            pxr::TfCallContext(__ARCH_FILE__, __ARCH_FUNCTION__, __LINE__, __ARCH_PRETTY_FUNCTION__),
            pxr::TF_DIAGNOSTIC_RUNTIME_ERROR_TYPE,
            "Unable to index PrimvarData due to element size greater than one"
        );
        return false;
    }

    // Compute the flattened values so that indexing can be performed on indexed or non-indexed data
    pxr::VtArray<T> flattenedValues;
    if (this->hasIndices())
    {
        flattenedValues.reserve(m_indices.size());
        for (const auto& index : m_indices)
        {
            if (size_t(index) < m_values.size())
            {
                flattenedValues.push_back(m_values[index]);
            }
            else
            {
                // Abort indexing if existing indices are outside the value range
                // this is a TF_RUNTIME_ERROR, but we have expanded the code manually to inject the class namespaces
                pxr::Tf_PostErrorHelper(
                    pxr::TfCallContext(__ARCH_FILE__, __ARCH_FUNCTION__, __LINE__, __ARCH_PRETTY_FUNCTION__),
                    pxr::TF_DIAGNOSTIC_RUNTIME_ERROR_TYPE,
                    "Unable to index PrimvarData due to existing indices outside the range of existing values"
                );
                return false;
            }
        }
    }
    else
    {
        flattenedValues = m_values;
    }

    // Compute the indices and indexed values
    pxr::VtArray<T> indexedValues;
    pxr::VtIntArray indices;
    indices.reserve(flattenedValues.size());

    std::unordered_map<size_t, int> indexMap;
    for (const auto& value : flattenedValues)
    {
        auto insertIt = indexMap.insert(std::make_pair(pxr::VtHashValue(value), static_cast<int>(indexedValues.size())));

        // If the insert succeeded it is a new value and should be added to the values array
        if (insertIt.second)
        {
            indexedValues.push_back(value);
        }

        indices.push_back(insertIt.first->second);
    }

    // Do not update the values and indices if their sizes have not changed.
    // Otherwise we are simply shuffling the data rather than actually changing the indexing.
    if (m_values.size() == indexedValues.size() && m_indices.size() == indices.size())
    {
        return false;
    }

    // Do not update the values and indices if the indices and values are the same size and the data is currently not indexed.
    // Otherwise we are authoring redundant indexing as there are no duplicate values.
    if (indexedValues.size() == indices.size() && m_indices.empty())
    {
        return false;
    }

    // Update the values and indices
    m_values = indexedValues;
    m_indices = indices;

    return true;
}

template <typename T>
bool PrimvarData<T>::operator==(const PrimvarData<T>& other) const
{
    return (
        (m_interpolation == other.interpolation()) && (m_elementSize == other.elementSize()) && (this->hasIndices() == other.hasIndices()) &&
        (m_values == other.values()) && (m_indices == other.indices())
    );
}

template <typename T>
bool PrimvarData<T>::operator!=(const PrimvarData<T>& other) const
{
    return !(*this == other);
}

} // namespace usdex::core
