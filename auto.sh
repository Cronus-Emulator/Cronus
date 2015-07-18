while true;
	do
	map=`ps aux | grep -v grep | grep -c map-server`
	login=`ps aux | grep -v grep | grep -c login-server`
	char=`ps aux | grep -v grep | grep -c char-server`
	if [ $map -lt 1 ]
	then
		sh cronus desligar
		sh cronus ligar
	elif [ $char -lt 1 ]
	then
		sh cronus desligar
		sh cronus ligar
	elif [ $login -lt 1 ]
	then
		sh cronus desligar
		sh cronus ligar
	fi
	sleep 120;
	done
