import os
lib_name = input("Enter library name: ")
lib_description = input("Enter library description: ")
src_dir = "src"
include_dir = "include"
cmake_file = "CMakeLists.txt"
idf_component_file = "idf_component.yml"
library_file = "library.json"
readme_file = "README.md"
header_file = lib_name + ".h"
src_file = lib_name + ".c"

readme_file_text = "# " + lib_name + "\n" + lib_description

cmake_file_text = ''''
idf_component_register(
    SRCS "src/''' + src_file +'''" 
    INCLUDE_DIRS "include"
    PRIV_REQUIRES logger err_controller memory_utils)'''

component_file_text = "dependencies:\n  idf: '>=5.0' \ndescription: " + lib_description + ".\nversion: 1.0.0" 

library_json_text = "{\n\t\"name\": \"" + lib_name + "\",\n\t\"version\": \"1.0.0\",\n\t\"keywords\": \"keyword\",\n\t\"description\": \"" + lib_description + "\",\n\t\"frameworks\": [\"espidf\"],\n\t\"platforms\": [\"espressif32\"]\n}"

src_c_file_text = "#include \"" + header_file + "\""

# mode 
mode = 0o777


os.chdir('../libraries')
libraries_path = os.getcwd()
print(libraries_path)

new_lib_path = os.path.join(libraries_path, lib_name)

cmake_file_path = os.path.join(new_lib_path, cmake_file)
idf_component_file_path = os.path.join(new_lib_path, idf_component_file)
library_file_path = os.path.join(new_lib_path, library_file)
header_file_path = os.path.join(new_lib_path,include_dir, header_file)
src_file_path = os.path.join(new_lib_path,src_dir, src_file)

os.mkdir(new_lib_path, mode)

os.chdir(new_lib_path)

os.makedirs(src_dir, mode)
os.makedirs(include_dir, mode)

f = open(readme_file, "w")
f.write(readme_file_text)
f.close()

f = open(cmake_file, "w")
f.write(cmake_file_text)
f.close()

f = open(idf_component_file, "w")
f.write(component_file_text)
f.close()

f = open(library_file, "w")
f.write(library_json_text)
f.close()

os.chdir("include")
print(os.getcwd())

f = open(header_file, "w")
f.close()

os.chdir("../src")
print(os.getcwd())

f = open(src_file, "w")
f.write(src_c_file_text)
f.close()


