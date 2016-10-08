:
while [ true ]
do
vim test.c
make
	if [ $? -eq 0 ]
	then
		#compilation was succesful
		echo "Compilation was successful"
		echo -n "Do you want to run program?(y/n): "
		read ANSWER
		if [ "$ANSWER" = "y" ] || [ "$ANSWER" = "Y" ]
		then
			#running program
			./test
		else
			#not running, returning to vim
			vim test.c
		fi
	fi
	echo -n "Exit?(y/n): "
	read X
	if [ "$X" = "y" ] || [ "$X" = "Y" ]
	then
		break
	fi
done

