#!/bin/bash

function create {
	echo "Enter working directory:";
	read dir;
	echo "Enter number of directories to create:";
	read dirnum;
	echo "Enter number of subdirectories to create:";
	read num;
	echo "Enter name pattern:";
	read pattern;

	cd $dir
	i=0;
	while [ "$i" -lt $dirnum ]
	do
		mkdir dir$i
		cd dir$i
		j=0;
		while [ "$j" -lt $num ]
		do
			mkdir subdir$j
			cd subdir$j
			k=0;
			while [ "$k" -lt 20 ]
			do
				touch $pattern$k
				k=$(($k+1));
			done
			cd ..
			j=$(($j+1));
		done
		cd ..
		i=$(($i+1));
	done
	exit;
}

function delete {
	echo "Enter working directory";
	read dir;
	echo "Enter directory numbers to delete, range or \"all\":";
	read dirs;
	echo "Enter name pattern of directories to delete:";
	read dirpat;
	echo "Enter subdirectory numbers to delete, range or \"all\":";
	read subdirs;
	echo "Enter name pattern of subdirectories to delete:";
	read subdirpat;
	echo "Enter file numbers to delete, range or \"all\":";
	read files;
	echo "Enter name pattern for files to delete:";
	read filepat;

	#удаляем директории
	cd $dir
	if [ "$dirs" = "all" ]
	then
		rm -r *
		exit;
	fi
	if [ `echo $dirs | grep -` = "$dirs" ]
	then
		r=`echo $dirs | sed "s/-/ /"`;
		range=`seq $r`;
	else
		range=$dirs;
	fi
	for i in $range
	do
		if [ -d "$dirpat$i" ]
		then
			rm -r $dirpat$i
		fi
	done
	
	#удаляем поддиректории
	for i in `ls`
	do
		if [ -d "$i" ]
		then
			cd $i
			if [ "$subdirs" = "all" ]
			then
				rm -r *
			else		
				if [ `echo $subdirs | grep -` = "$subdirs" ]
				then
					r=`echo $subdirs | sed "s/-/ /"`;
					range=`seq $r`;
				else
					range=$subdirs;
				fi
				for i in $range
				do
					if [ -d "$subdirpat$i" ]
					then
						rm -r $subdirpat$i
					fi
				done
			fi
			cd ..
		fi
	done

	#удаляем файлы
	for i in `ls`
	do
		if [ -d "$i" ]
		then
			cd $i
			for j in `ls`
			do
				if [ -d "$j" ]
				then
					cd $j				
					if [ `echo $files | grep -` = "$files" ]
					then
						r=`echo $files | sed "s/-/ /"`;
						range=`seq $r`;
					else
						range=$files;
					fi
					for f in $range
					do
						rm $filepat$f
					done
				fi
			done
		fi
	done
}

if [ "$#" -ne 1 ]
then
	echo "You should pass 1 parameter: \"create\" or \"delete\"";
	exit;
fi

if [ "$1" = "create" ]
then
	create	
else if [ "$1" = "delete" ]
then
	delete
fi
fi

exit;
