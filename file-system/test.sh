#!/bin/sh

echo "================== Compiling ===================================="

make clean
make all

echo "============= Creating and Removing Drive ========================"
echo "\nTry to create too small drive"
./fs c D 800

echo "\nCreate drive"
./fs c D 10000
echo "\nls -l D"
ls -l D
echo "\nDrive info:"
./fs i D
echo "\n"
./fs r D
echo "\n"

echo "==================== Saving files to Drive ======================="

./fs c D 1000
echo "\nDrive info: "
./fs i D
echo "\n"

for i in 1 2 3 4 5 6
do
	./fs s D test-files/file$i.txt

	#echo "\nDrive info:"
	#./fs i D
	echo "\n"
	./fs l D
	./fs m D
	echo "\n"
done

echo "=============== Coping file with same name ========================"
echo "\n"
./fs s D test-files/file1.txt
./fs l D
echo "\n"

echo "================ Deleting and saving new files ====================="
echo "\n"
./fs d D test-files/file3.txt
./fs d D test-files/file4.txt
./fs s D test-files/file6.txt
echo "\nDrive info:"
./fs i D
echo "\n"
./fs l D
./fs m D
echo "\n"

./fs s D test-files/file7.txt

echo "============== Coping file from drive (diff check) ================="
echo "\nFile to copy:"
ls -l test-files/cat.jpg

echo "\nCreate drive:"
./fs c D 30000

echo "\nSave file to drive:"
./fs s D test-files/cat.jpg

echo "\nDrive info:"
./fs i D
echo "\n"
./fs l D

echo "\nCopy file from drive:"
mv test-files/cat.jpg test-files/cat.jpg_org
./fs p D test-files/cat.jpg

echo "\nCheck for difference:"
diff test-files/cat.jpg_org test-files/cat.jpg
ret=$?

if [ $ret -eq 0 ]; then
    echo "passed.\n"
else
    echo "failed.\n"
fi

./fs r D

echo "\nCleaning..."
rm test-files/cat.jpg
mv test-files/cat.jpg_org test-files/cat.jpg 
make clean 

echo "==================================================================="
