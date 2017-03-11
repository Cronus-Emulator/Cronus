while true;
	do
	services=`ps aux | grep -v grep | grep -cE "map-server|login-server|char-server"`
	if [ $services -lt 3 ]; then
		sh cronus desligar
		sh cronus ligar
	sleep 120;
	done
