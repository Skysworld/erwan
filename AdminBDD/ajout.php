<?php
	$Nom=$_POST['NomAjout'];
	$Mac=$_POST['AdresseMacAjout'];
	$bdd = new PDO('mysql:host=localhost;dbname=domotique;charset=utf8', 'root', '');
	$zero=0;
	$reponse = $bdd->query("INSERT INTO prise(Identifiant,Mac,IDWeb,EtatBool1,EtatBool2,EtatBool3,EtatBool4,EtatBool5) VALUES('$Nom','$Mac','$zero','$zero','$zero','$zero','$zero','$zero')");
	echo "Prise $Mac ,ajouter a $Nom";
	echo("<br><p><a href='page.html'>Accueil</a></p>")
?>
