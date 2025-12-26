multigpu_sources = [
    'nvidia_device_group.c',
    ]

    multigpu_headers = [
        'nvidia_device_group.h',
            'multigpu.h',
            ]

            libvkd3d_multigpu = static_library(
                'vkd3d-multigpu',
                    multigpu_sources,
                        dependencies : [vulkan_dep, vkd3d_common_dep],
                            include_directories : vkd3d_include_path,
                                install : false,
                                )

                                vkd3d_multigpu_dep = declare_dependency(
                                    link_with : libvkd3d_multigpu,
                                        include_directories : include_directories('.'),
                                        )
