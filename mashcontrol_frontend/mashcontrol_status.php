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

draw_refresh_button($logfile);

draw_kill_button($logfile);

draw_continue_button($logfile);


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
