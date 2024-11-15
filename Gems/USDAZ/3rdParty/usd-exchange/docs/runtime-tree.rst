.. tab-set::

    .. tab-item:: Linux Default
        :sync: linux

        .. code-block:: bash
            :caption: Note the python runtime is optional, as is the `usdex_rtx` library and module.

            ├── lib
            │   ├── libusdex_core.so
            │   ├── libusdex_rtx.so
            │   ├── libomni_transcoding.so
            │   ├── libboost_python310.so -> libboost_python310.so.1.78.0
            │   ├── libboost_python310.so.1.78.0
            │   ├── libpython3.10.so -> libpython3.10.so.1.0
            │   ├── libpython3.10.so.1.0
            │   ├── libpython3.so
            │   ├── libtbb.so.2
            │   ├── libusd_arch.so
            │   ├── libusd_ar.so
            │   ├── libusd_gf.so
            │   ├── libusd_js.so
            │   ├── libusd_kind.so
            │   ├── libusd_ndr.so
            │   ├── libusd_pcp.so
            │   ├── libusd_plug.so
            │   ├── libusd_sdf.so
            │   ├── libusd_sdr.so
            │   ├── libusd_tf.so
            │   ├── libusd_trace.so
            │   ├── libusd_usdGeom.so
            │   ├── libusd_usdLux.so
            │   ├── libusd_usdShade.so
            │   ├── libusd_usd.so
            │   ├── libusd_usdUtils.so
            │   ├── libusd_vt.so
            │   ├── libusd_work.so
            |   └── usd
            |       ├── plugInfo.json
            |       └── ...
            |           └── resources
            |               └── plugInfo.json
            ├── python
            |   ├── pxr
            │   |   └── ...
            |   └── usdex
            |       ├── core
            |       │   ├── __init__.py
            |       │   ├── _StageAlgoBindings.py
            |       │   ├── _usdex_core.cpython-310-x86_64-linux-gnu.so
            |       │   └── _usdex_core.pyi
            |       └── rtx
            |           ├── __init__.py
            |           ├── _usdex_rtx.cpython-310-x86_64-linux-gnu.so
            |           └── _usdex_rtx.pyi
            └── python-runtime
                ├── bin
                ├── lib
                └── ...

    .. tab-item:: Linux `usd-minimal`

        .. code-block:: bash
            :caption: Note OpenUSD is a minimal build with compact dependencies. The `usdex_rtx` library is optional.

            └── lib
                ├── libusdex_core.so
                ├── libusdex_rtx.so
                ├── libomni_transcoding.so
                ├── libtbb.so.2
                ├── libusd_ms.so
                └── usd
                    ├── plugInfo.json
                    └── ...
                        └── resources
                            └── plugInfo.json

    .. tab-item:: Windows Default
        :sync: windows

        .. code-block:: bash
            :caption: Note the python runtime is optional, as is the `usdex_rtx` library and module.

            ├── lib
            │   ├── usdex_core.dll
            │   ├── usdex_rtx.dll
            │   ├── omni_transcoding.dll
            |   ├── boost_python310-vc142-mt-x64-1_78.dll
            │   ├── python3.dll
            │   ├── python310.dll
            │   ├── tbb.dll
            │   ├── usd_ar.dll
            │   ├── usd_arch.dll
            │   ├── usd_gf.dll
            │   ├── usd_js.dll
            │   ├── usd_kind.dll
            │   ├── usd_ndr.dll
            │   ├── usd_pcp.dll
            │   ├── usd_plug.dll
            │   ├── usd_sdf.dll
            │   ├── usd_sdr.dll
            │   ├── usd_tf.dll
            │   ├── usd_trace.dll
            │   ├── usd_usd.dll
            │   ├── usd_usdGeom.dll
            │   ├── usd_usdLux.dll
            │   ├── usd_usdShade.dll
            │   ├── usd_usdUtils.dll
            │   ├── usd_vt.dll
            │   └── usd_work.dll
            |   └── usd
            |       ├── plugInfo.json
            |       └── ...
            |           └── resources
            |               └── plugInfo.json
            ├── python
            |   ├── pxr
            │   |   └── ...
            |   └── usdex
            |       ├── core
            |       │   ├── __init__.py
            |       │   ├── _StageAlgoBindings.py
            |       |   ├── _usdex_core.cp310-win_amd64.pyd
            |       │   └── _usdex_core.pyi
            |       └── rtx
            |           ├── __init__.py
            |           ├── _usdex_rtx.cp310-win_amd64.pyd
            |           └── _usdex_rtx.pyi
            └── python-runtime
                ├── bin
                ├── lib
                └── ...

    .. tab-item:: Windows `usd-minimal`

        .. code-block:: bash
            :caption: Note OpenUSD is a minimal build with compact dependencies. The `usdex_rtx` library is optional.

            └── lib
                ├── usdex_core.dll
                ├── usdex_rtx.dll
                ├── omni_transcoding.dll
                ├── tbb.dll
                ├── usd_ms.dll
                └── usd
                    ├── plugInfo.json
                    └── ...
                        └── resources
                            └── plugInfo.json
