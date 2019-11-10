LD_LIBRARY_PATH=/usr/local/lib
D_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
export LD_LIBRARY_PATH
echo "Medians excecution times with different opti option" > median_out.txt

#echo -e "\nNone:\n" &>> median_out.txt
#g++ `pkg-config --cflags opencv4` medians.cpp `pkg-config --libs opencv4` -lpthread -o medians
#./medians &>> median_out.txt
#echo "----------------------------------------------------------------------------------" &>> median_out.txt

#echo -e "\nO1:\n" &>> median_out.txt
#g++ `pkg-config --cflags opencv4` medians.cpp `pkg-config --libs opencv4` -lpthread -o medians -O1
#./medians &>> median_out.txt
#echo "----------------------------------------------------------------------------------" &>> median_out.txt

echo -e "\nO2:\n" &>> median_out.txt
g++ `pkg-config --cflags opencv4` medians.cpp `pkg-config --libs opencv4` -lpthread -o medians -O2
./medians &>> median_out.txt
rm medians
cat median_out.txt
