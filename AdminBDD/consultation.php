<?php
		$serveur = "localhost";
		$base = "domotique";
		$user = "root";
		$pass = "";
		$mysqli = new mysqli($serveur, $user, $pass, $base);
		if ($mysqli->connect_error) {
		die('Erreur de connexion ('.$mysqli->connect_errno.')'. $mysqli->connect_error);}
		
		
		//$requete="SELECT Identifiant FROM identifiant";
		$requete="SELECT Identifiant,Mac FROM prise";
		$resultat = $mysqli->query($requete) or die ('Erreur '.$requete.' '.$mysqli->error());
		
		while($row = $resultat->fetch_assoc())
		{
			 echo ("Utilisateur $row[Identifiant] as la prise suivante : $row[Mac] <br>");
		}
		echo("<br><p><a href='page.html'>Accueil</a></p>")
?>