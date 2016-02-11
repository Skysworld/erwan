<?php
// prevent the server from timing out
set_time_limit(0);

// include the web sockets server script (the server is started at the far bottom of this file)
require 'class.PHPWebSocket.php';

// when a client sends data to the server
function wsOnMessage($clientID, $message, $messageLength, $binary) {
	global $Server;
	if (is_numeric($message[0])==true || ctype_alpha($message[0])==true)
	{
		$bdd = new PDO('mysql:host=localhost;dbname=domotique;charset=utf8', 'root', '');
		$reponse = $bdd->query("UPDATE prise SET IDWeb = '$clientID' WHERE Mac ='$message'");
		$Server->log("Enregistrement a ladresse : $message de l ID : $clientID");
	}

	
	//Si on envoie addresse mac et et message envoief4:b5
	if($message[0]=='#')
	{
		$Mac=  substr($message, 1, 19);
		$bdd = new mysqli("localhost", "root", "", "domotique");
		$reponseID = $bdd->query("SELECT IDWeb FROM prise WHERE Mac ='$Mac'");
		$reponseBool = $bdd->query("SELECT EtatBool1, EtatBool2, EtatBool3,EtatBool4 ,EtatBool5 FROM prise WHERE Mac='$Mac'");
		$IDrecup = $reponseID->fetch_row();		
		$Etatrecup = $reponseBool->fetch_row();//Envoyer les donnes sout forme d'un octet
		$Donne="$Etatrecup[0]$Etatrecup[1]$Etatrecup[2]$Etatrecup[3]$Etatrecup[4]";//Concatenation de la chaine de donne
		$Server->wsSend($IDrecup[0], "$Donne");
		$Server->log("Information Envoyer: $Donne");
	}
}

// when a client connects
function wsOnOpen($clientID)
{
	global $Server;
	$ip = long2ip( $Server->wsClients[$clientID][6] );
	$Server->log( "$ip ($clientID) has connected." );
  
}

// when a client closes or lost connection
function wsOnClose($clientID, $status) {
	global $Server;
	$ip = long2ip( $Server->wsClients[$clientID][6] );
	$Server->log( "$ip ($clientID) is disconnected." );
}

// start the server
$Server = new PHPWebSocket();
$Server->bind('message', 'wsOnMessage');
$Server->bind('open', 'wsOnOpen');
$Server->bind('close', 'wsOnClose');
$Server->log( "Serveur lance" );
// for other computers to connect, you will probably need to change this to your LAN IP or external IP,
// alternatively use: gethostbyaddr(gethostbyname($_SERVER['SERVER_NAME']))
$Server->wsStartServer('0.0.0.0', 9300);


?>