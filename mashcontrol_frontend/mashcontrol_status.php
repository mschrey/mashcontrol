<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="refresh" content="10" > 
</head>

<body>
<h3>Mashcontrol - beta</h3>

<?php
include 'mashcontrol_inc.php';

$logfile = $_GET['logfile'];

echo "<form action=\"mashcontrol_status.php\" method=\"get\">
<input type=\"hidden\" name=\"function\" value=\"status\">
<input type=\"hidden\" name=\"logfile\" value=$logfile>
<input type=\"submit\" value=\"Refresh Status\">
</form>";

echo "<form action=\"mashcontrol_status.php\" method=\"get\">
<input type=\"hidden\" name=\"function\" value=\"kill\">
<input type=\"hidden\" name=\"logfile\" value=$logfile>
<input type=\"submit\" value=\"Kill mashcontrol\">
</form>";

echo "<form action=\"mashcontrol_status.php\" method=\"get\">
<input type=\"hidden\" name=\"function\" value=\"continue\">
<input type=\"hidden\" name=\"logfile\" value=$logfile>
<input type=\"submit\" value=\"Continue\">
</form>";

if (isset($_GET['function']) and $_GET['function'] == 'status'){
   get_current_status($path.$_GET['logfile']);
}
    
if (isset($_GET['function']) and $_GET['function'] == 'continue'){
   send_sigusr1_signal();
}

if (isset($_GET['function']) and $_GET['function'] == 'kill'){
    exec('killall mashcontrol');
    echo "killed the program<br>";
}



//print_get_request();


?>

<a href="mashcontrol.php">BACK</a>
</body>
</html>
