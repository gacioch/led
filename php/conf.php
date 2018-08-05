<?php
$host = "192.168.4.1";
$port = 25005;
//tworzenie(jesli nie istnieje) i laczanie z gniazdem do komunikacji z programem w C
$socket=socket_create(AF_INET, SOCK_STREAM, 0);
$result=socket_connect($socket, $host, $port);

// odebranie komunikatu przychodzacego z aplikacji mobilnej, odkodowanie i przeslanie do programu w C
// komunikat o wlaczeniu lub wylaczenuiu led
if($wlaczanie = $_GET["wl"]){
	$wlaczanie2 = json_encode($wlaczanie);
	if(isset($wlaczanie2)) socket_write($socket, $wlaczanie2, strlen($wlaczanie2));
}
// analogicznie jak wyzej
// kolor w postaci r g b 
if($color = $_GET["kolor"]){
	$color2=json_encode($color);
	if(isset($color2)) socket_write($socket, $color2, strlen($color2));
}
// analogicznie jak wyzej
// 
if($flaga = $_GET["flaga"]){
	$flaga2 = json_encode($flaga);
	if(isset($flaga2)) socket_write($socket, $flaga2, strlen($flaga2));
}
// analogicznie jak wyzej
// punkty do kalibracji ekranu
if($kalibracja = $_GET["kalibracja"]){
	$kalibracja2 = json_encode($kalibracja);
	if(isset($kalibracja2)) socket_write($socket, $kalibracja2, strlen($kalibracja2));
}
// analogicznie jak wyzej
// zmienne do konfiguracji hotspota
if($op = $_GET["op"]){
	$op2 = json_encode($op);
	if(isset($op2)) socket_write($socket, $op2, strlen($op2));
}

?>
