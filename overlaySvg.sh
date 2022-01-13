# Takes a SVG file then creates an ncode file and a BMP vizualization of the drawing

image_file=$1
ncode_save_file=out/overlaySvg.ncode
test_save_file=out/overlaySvg.bmp
viz_save_file=out/overlayVizSvg.bmp
overlap_save_file=out/overlapped.bmp

# Remove the previous save file
rm $save_file

# Define the commands
parse_svg_command="python ./ImageProcessor/svg_parser.py $image_file $ncode_save_file"
viz_command="python ./Tools/NcodeVizualizer.py $ncode_save_file $viz_save_file"
test_command="./CTests/draw $ncode_save_file"
overlap_command="python Tools/OverlapVizTest.py $test_save_file $viz_save_file"

printf "Translating $image_file into $save_file\n"
if (cd CTests && make draw); then
    if $parse_svg_command; then  # Turn the SVG into ncode
        printf "\nVizualizing $save_file\n"
        if $viz_command; then  # Vizualize the ncode
            if $test_command; then  # Test the ncode
                if $overlap_command; then
                    printf "Successfully overlapped SVG test and viz files"
                    code $overlap_save_file
                else    
                    printf "Failed to overlap files"
                fi
            else
                printf "Failed to test $save_file\n"
            fi
        else
            printf "Failed to vizualize $save_file\n"
        fi
    else
        printf 'Failed to parse SVG file\n'
    fi
else
    printf "\nFailed to build draw"
fi
