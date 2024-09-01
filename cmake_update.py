import os
import re

def get_all_file_pathes():
    result = []

    root_dir = os.getcwd()

    curr_directory_path_length = len(re.split(r'[\\\.]', root_dir))

    for root, dirs, files in os.walk(root_dir):
        # Create a list for the current directory's files
        current_files = [os.path.join(root, file) for file in files]
        for current_file in current_files:
            current_file_splitted = re.split(r'[\\\.]', current_file)
            current_file_splitted = current_file_splitted[curr_directory_path_length:]
            # Append the list of file paths to the result
            result.append(current_file_splitted)

    return result

def get_all_directory_pathes():

    root_dir = os.getcwd()

    curr_directory_path_length = len(re.split(r'[\\\.]', root_dir))

    directory_paths = []
    result = []

    for dirpath, dirnames, filenames in os.walk(root_dir):
        for dirname in dirnames:
            directory_paths.append(os.path.join(dirpath, dirname))

    for current_directory in directory_paths:
            current_directory_splitted = re.split(r'[\\\.]', current_directory)
            current_directory_splitted = current_directory_splitted[curr_directory_path_length:]
            # Append the list of file paths to the result
            result.append(current_directory_splitted)

    return result

def update_shaders():
    print("update_shaders")
    result = get_all_file_pathes()

    resut_filtered = [file for file in result if (('hlsl' in file) 
                                                   #or ('hlsli' in file) 
                                                     )]

    with open('result.txt', "w") as the_file:

        the_file.write("#" * 69 + "\n" + "# SHADERS_FILES_START\n" + "#" * 69 + "\n")

        #for line in resut_filtered:
        #    the_file.write(f"{line}\n")
        #the_file.write("\n")

        the_file.write("\n" + "#List of HLSL shaders" + "\n")
        the_file.write(f"{'set(SHADER_NAMES'}\n")
        for line in resut_filtered:
            shader_name = line[-2]
            shader_name.replace("'", '')
            shader_name = '"' + shader_name + ".cso" + '"'
            the_file.write(f"    {shader_name}\n")
        the_file.write(f"{')'}\n")

        the_file.write("\n" + "# List of shader settings" + "\n")
        the_file.write(f"{'set(HLSL_SHADERS'}\n")
        for line in resut_filtered:
            length = len(line)
            shader_type = line[-2][-2:]

            path_front = r'    "${CMAKE_CURRENT_SOURCE_DIR}'
            for word in line[:-2]:
                path_front += "/" + word

            if(shader_type == 'PS'):
                path_front += "/" + line[-2] + ".hlsl,5.0,Pixel,main" + '"'

            if(shader_type == 'VS'):
                path_front += "/" + line[-2] + ".hlsl,5.0,Vertex,main" + '"'

            the_file.write(f"{path_front}\n")

        the_file.write(f")\n")

        the_file.write("\n" + "#" * 69 + "\n" + "# SHADERS_FILES_END\n" + "#" * 69 + "\n")


    return

def update_folder_structure():
    print("update_folder_structure")

    result = get_all_directory_pathes()

    resut_filtered =  [file for file in result if not (('build' in file) 
                                                     )]

    with open('result2.txt', "w") as the_file:
 
        the_file.write("\n" + "#" * 69 + "\n" + "# SOURCE_GROUPS_START\n" + "#" * 69 + "\n")

        # Real folders
        for line in resut_filtered:
            
            root_dir = os.getcwd()
            folder_dir = ""

            for path in line:
                root_dir += "/" + path
            filenames = [f for f in os.listdir(root_dir) if os.path.isfile(os.path.join(root_dir, f))]

            if not filenames: 
                continue

            for path in line:
                if(path == line[-1]):
                    folder_dir += path
                else:
                    folder_dir += path + "__" 

            folder_dir_slash = folder_dir.replace("__", "/")
            folder_dir_baclslash = folder_dir.replace("__", "\\\\")

            the_file.write("\n" + "set(")
            the_file.write(f"{folder_dir}\n")

            for file in filenames:
                the_file.write("    " + '"' + f"{folder_dir_slash}" + "/" + f"{file}"+ '"' + "\n")
            the_file.write(")" + "\n")
            the_file.write("source_group(" + '"' + f"{folder_dir_baclslash}" + '"' + " FILES " + "${" + f"{folder_dir}" + "})" "\n")

        # VS folders
        the_file.write("\n" + "set(ALL_FILES" + "\n")
        the_file.write("    ${no_group_source_files}" + "\n")
        the_file.write("    ${External__DirectXTex}" + "\n")
        the_file.write("    ${External__imgui-docking}" + "\n")

        for line in resut_filtered:
            
            the_file.write("    $" + "{")
            for path in line:
                if(path == line[-1]):
                    the_file.write(f"{path}")
                else:
                    the_file.write(f"{path}" + "__" )
            the_file.write("}" + "\n")
        
        the_file.write(")" + "\n")

        the_file.write("\n" + "#" * 69 + "\n" + "# SOURCE_GROUPS_END\n" + "#" * 69 + "\n")

    return

def main():
    print("1. Update shaders")
    print("2. Update folder structure")
    print("3. Update all")
    print("4. Exit")

    get_all_file_pathes()

    choose_script = input()
    if(choose_script == '1'):
        update_shaders()
    if(choose_script == '2'):
        update_folder_structure()
    if(choose_script == '3'):
        update_shaders()
        update_folder_structure()
    if(choose_script == '4'):
        return

    return

if __name__ == "__main__":
    main()