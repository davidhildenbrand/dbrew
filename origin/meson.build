
subdir('src')
dbrew = declare_dependency(include_directories: include_directories('include'),
                           link_with: libdbrew)
dbrew_priv = declare_dependency(include_directories: dbrew_includes,
                                link_with: libdbrew)

install_headers('include/dbrew.h')
