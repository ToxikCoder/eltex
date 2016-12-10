#!/bin/bash
DIALOG=${DIALOG=dialog}

temp=`mktemp 2>/dev/null` || temp=/temp/test$$
trap "rm -f $temp" 0 1 2 5 15

$DIALOG --clear --backtitle "Лаунчер лабораторных работ" \
	--title "Выберите лабораторную работу" \
	--menu "Лабораторная" 20 61 11 \
	"Lab1[x]" "Основы C и Bash" \
	"Lab2[x]" "Функции и указатели в C" \
	"Lab3[x]" "Структуры и массивы" \
	"Lab4[x]" "Файловые операции" \
	"Lab5[x]" "Статические и динамические библиотеки" \
	"Lab6[x]" "Процессы в Linux" \
	"Lab7" "Каналы" \
	"Lab8" "Очереди сообщений" \
	"Lab9" "Семафоры и разделяемая память" \
	"Lab10" "Потоки" \
	"Lab11" "Сетевое программирование в Linux" 2> $temp

res=`cat $temp`
clear
if [ "$res" == "Lab1[x]" ]
then
	#make and run lab1
	cd ./lab1
	make
	if [ $? -eq 0 ]
	then
		./test
	fi
	make clean
	cd ..
elif [ "$res" == "Lab2[x]" ]
then
	#make and run lab2
	cd ./lab2
	make
	if [ $? -eq 0 ]
	then
		./sort
	fi
	make clean
	cd ..
elif [ "$res" == "Lab3[x]" ]
then
	#make and run lab3
	cd ./lab3
	make
	if [ $? -eq 0 ]
	then
		./lab < data
	fi
	make clean
	cd ..
elif [ "$res" == "Lab4[x]" ]
then
	#make and run lab4
	cd ./lab4
	make
	if [ $? -eq 0 ]
	then
		./lab -i data -o file.out -m BY_STR
	fi
	make clean
	cd ..
elif [ "$res" == "Lab5[x]" ]
then
	#make and run lab5
	cd ./lab5
	make static
	if [ $? -eq 0 ]
	then
		./static
	fi
	make dynamic
	if [ $? -eq 0 ]
	then
		./dynamic
	fi
	make clean
	cd ..
elif [ "$res" == "Lab6[x]" ]
then
	#make and run lab6
	cd ./lab6
	make
	if [ $? -eq 0 ]
	then
		./funcproc
	fi
	make clean
	cd ..
elif [ "$res" == "Lab7[x]" ]
then
	#make and run lab7
	cd ./lab7
	make
	if [ $? -eq 0 ]
	then
		./lab
	fi
	make clean
	cd ..
elif [ "$res" == "Lab8[x]" ]
then
	#make and run lab8
	cd ./lab8
	make
	if [ $? -eq 0 ]
	then
		./lab
	fi
	make clean
	cd ..
elif [ "$res" == "Lab9[x]" ]
then
	#make and run lab9
	cd ./lab9
	make
	if [ $? -eq 0 ]
	then
		./lab
	fi
	make clean
	cd ..
elif [ "$res" == "Lab10[x]" ]
then
	#make and run lab10
	cd ./lab10
	make
	if [ $? -eq 0 ]
	then
		./lab
	fi
	make clean
	cd ..
elif [ "$res" == "Lab11[x]" ]
then
	#make and run lab11
	cd ./lab11
	make
	if [ $? -eq 0 ]
	then
		./lab
	fi
	make clean
	cd ..
else
	echo "Эта лаба ещё не готова"
fi
echo "До свидания..."

