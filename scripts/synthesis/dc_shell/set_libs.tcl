# add std cells .db files
set DB_STDCELLS {<your_db_files_here>}

set search_path [list $search_path . <your_path_here>]
set link_library [list "*" "<your_library_here>"]

# target library
set target_library [list "<your_library_here>" ]
set synthetic_library [list "<your_library_here>" ]


# link library
set link_library "* $target_library"

#debug output info
puts "------------------------------------------------------------------"
puts "USED LIBRARIES"
puts $link_library
puts "------------------------------------------------------------------"

set link_library " * $link_library"
