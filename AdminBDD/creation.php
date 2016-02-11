<?php
	$Nom=$_POST['NomCreation'];
	$Mac=$_POST['AdresseMacCreation'];
	$Pass=$_POST['PasswordCreation'];
	$Pass=sha1($Pass);
	$bdd = new PDO('mysql:host=localhost;dbname=domotique;charset=utf8', 'root', '');
	$zero=0;
	$bdd->query("INSERT INTO identifiant(Identifiant,Password) VALUES('$Nom','$Pass')");
	$bdd->query("INSERT INTO prise(Identifiant,Mac,IDWeb,EtatBool1,EtatBool2,EtatBool3,EtatBool4,EtatBool5) VALUES('$Nom','$Mac','$zero','$zero','$zero','$zero','$zero','$zero')");
	echo "Profil crée $Nom avec la Prise $Mac ajouter.";
	echo("<br><p><a href='page.html'>Accueil</a></p>")
?>
;