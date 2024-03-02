import os
from datetime import date
 
error_files = []
bases = {}
defines = 0

today = str(date.today())
os.chdir("..")

lookup_table_beg = """/**
 * @file err_c_names.c
 * @author Wojciech Mytych
 * @brief Auto generated file with ESP Helper libraries error names.
 * @version 1.0.3
 * @date """ + today + """
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "err_controller.h"
#include <string.h>

#ifdef ESP_PLATFORM
#include <esp_err.h>
#else
#include <errno.h>
#endif

const char* error_to_name(err_c_t err) {
    switch (err)
    {
"""

lookup_table_end = """    default:
        #ifdef ESP_PLATFORM
        return esp_err_to_name(err);
        #else
        err = errno;
        return strerror(err);
        #endif
    }
    return "i don't know?";
}
"""
with open("lib/err_controller/src/err_c_names.c", 'w+') as t:
    t.write(lookup_table_beg)


#first get list of files containing errors
for lib in os.listdir("lib"):
    if(lib != "CException" and lib != "README"):
        os.chdir("lib/" + lib + "/include")
        #print(os.getcwd())
        for header in os.listdir():
            if(header.endswith("_errors.h")):
                error_files.append(header)
                with open(header, 'r') as fp:
                    lines = fp.readlines()
                    for row in lines:
                        define = '#define'
                        if row.find(define) != -1:
                            print('Found new defines: ' + row)
                            defines = defines + 1
                            without_com = row.split("///< ", 1)
                            comment = without_com[1]
                            comment = comment.strip()
                            print("error name: " + comment)
                            number = without_com[0].split("0x")
                            number = number[1]
                            if row.find("+") == -1:
                                base_name = without_com[0]
                                base_name = base_name.split("#define")
                                base_name = base_name[1].split("0x")
                                base_name = base_name[0].strip()
                                print("base name: " + base_name)
                                bases[base_name] = number.strip()
                                base = 'none'
                            else:
                                base = number.split("+")
                                number = base[0].strip()
                                base = base[1].strip()
                            print("Error number: 0x" + number)
                            print("Error base: " + base)
                            with open("../../../lib/err_controller/src/err_c_names.c", 'a+') as t:
                                if(base != "none"):
                                    t.write('\tcase 0x' + number + ' + 0x' + bases[base] + ':\n\t\treturn "' + comment + '";\n')
                                else:
                                    continue
        os.chdir("../../..")
        
with open("lib/err_controller/src/err_c_names.c", 'a+') as t:
    t.write(lookup_table_end)


print("\nscanned " + str(len(error_files)) + " files for errors")
print("found " + str(defines-len(bases)) + " defined errors")
print("found " + str(len(bases)) + " defined error bases")