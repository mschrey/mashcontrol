<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
</head>

<body>
<h3>Mashcontrol - beta</h3>

<form action="maischcontroller.php" method="get" >
<p>Rezeptname:
<input type="text" name="recipe_name">
</p>

<table border="0">
<tr><td>
Rast 1:
<input type="checkbox" name="step1" value="Yes" checked="checked"> 
<input type="hidden" name="step1_name" value="Einmaischen">
Einmaischen: </td><td>Temperatur <input type="text" name="step1_temp" value="60" size="5">&deg;C</td><td>
<input type="hidden" name="step1_duration" value="0"></td></tr>

<tr><td>
Rast 2:
<input type="checkbox" name="step2" value="Yes"> 
<input type="hidden" name="step2_name" value="Eiweissrast">
Eiweissrast: </td><td>Temperatur <input type="text" name="step2_temp" value="57" size="5">&deg;C</td><td>
Dauer:
<select name="step2_duration">
<option value="10">10</option>
<option value="15">15</option>
<option value="20">20</option>
<option value="25">25</option>
<option value="30">30</option>
<option value="35">35</option>
<option value="40">40</option>
<option value="45">45</option>
<option value="50">50</option>
<option value="55">55</option>
<option value="60">60</option>
<option value="70">70</option>
<option value="80">80</option>
<option value="90">90</option>
</select></td></tr>


<tr><td>
Rast 3:
<input type="checkbox" name="step3" value="Yes" checked="checked"> 
<input type="hidden" name="step3_name" value="Maltoserast">
Maltoserast: </td><td>Temperatur <input type="text" name="step3_temp" value="63" size="5">&deg;C</td><td>
Dauer:
<select name="step3_duration">
<option value="10">10</option>
<option value="15">15</option>
<option value="20">20</option>
<option value="25">25</option>
<option value="30">30</option>
<option value="35" selected="selected">35</option>
<option value="40">40</option>
<option value="45">45</option>
<option value="50">50</option>
<option value="55">55</option>
<option value="60">60</option>
<option value="70">70</option>
<option value="80">80</option>
<option value="90">90</option>
</select></td></tr>


<tr><td>
Rast 4:
<input type="checkbox" name="step4" value="Yes"> 
<input type="hidden" name="step4_name" value="Kombirast">
Kombirast:  </td><td>Temperatur <input type="text" name="step4_temp" value="68" size="5">&deg;C</td><td>
Dauer:
<select name="step4_duration">
<option value="10">10</option>
<option value="15">15</option>
<option value="20">20</option>
<option value="25">25</option>
<option value="30">30</option>
<option value="35">35</option>
<option value="40">40</option>
<option value="45">45</option>
<option value="50">50</option>
<option value="55">55</option>
<option value="60" selected="selected">60</option>
<option value="70">70</option>
<option value="80">80</option>
<option value="90">90</option>
</select></td></tr>


<tr><td>
Rast 5:
<input type="checkbox" name="step5" value="Yes" checked="checked"> 
<input type="hidden" name="step5_name" value="Verzuckerungsrast">
Verzuckerungsrast:  </td><td>Temperatur <input type="text" name="step5_temp" value="72" size="5">&deg;C</td><td>
Dauer:
<select name="step5_duration">
<option value="10">10</option>
<option value="15">15</option>
<option value="20" selected="selected">20</option>
<option value="25">25</option>
<option value="30">30</option>
<option value="35">35</option>
<option value="40">40</option>
<option value="45">45</option>
<option value="50">50</option>
<option value="55">55</option>
<option value="60">60</option>
<option value="70">70</option>
<option value="80">80</option>
<option value="90">90</option>
</select></td></tr>


<tr><td>
Rast 6:
<input type="checkbox" name="step6" value="Yes" checked="checked"> 
<input type="hidden" name="step6_name" value="Abmaischen">
Abmaischen:  </td><td>Temperatur <input type="text" name="step6_temp" value="78" size="5">&deg;C</td><td>
<input type="hidden" name="step6_duration" value="0"></td></tr>
</table>


<input type="hidden" name="function" value="start">
<input type="submit" value="Start Maischcontroller">
</form>


<form action="maischcontroller.php" method="get">
<input type="hidden" name="function" value="kill">
<input type="submit" value="Kill Maischcontroller">
</form>




<?php
include 'maischcontroller_include.php';

if (isset($_GET['recipe_name'])){
    $recipe_name = $_GET['recipe_name'];   
    $date = date('Y-m-d');
    $filename = $date."_".cleanup_recipe_name($recipe_name).".msf";
    $logfile = $date."_".cleanup_recipe_name($recipe_name)."";
//    echo "filename is ".$filename."<br>";
//    echo "full path is ".$path.$filename."<br>";
    
    $file = fopen($path.$filename, "w");
    if ($file == false) {
        echo "Error opening file! Exiting!";
        exit();
    }



echo "<form action=\"maischcontroller_status.php\" method=\"get\">
<input type=\"hidden\" name=\"function\" value=\"status\">
<input type=\"hidden\" name=\"logfile\" value=$logfile>
<input type=\"submit\" value=\"Refresh Status\">
</form>";


    
    fwrite($file, "# Maischprogramm fuer ".$recipe_name."\n");
    fwrite($file, "# created by Maischcontroller web frontend on ".$date."\n");
    if ($_GET['step1'] == 'Yes') {
        print_step($_GET['step1_name'], $_GET['step1_temp'], $_GET['step1_duration'], $file);
    }
    if (isset($_GET['step2']) and $_GET['step2'] == 'Yes') {
        print_step($_GET['step2_name'], $_GET['step2_temp'], $_GET['step2_duration'], $file);
    }
    if (isset($_GET['step3']) and $_GET['step3'] == 'Yes') {
        print_step($_GET['step3_name'], $_GET['step3_temp'], $_GET['step3_duration'], $file);
    }
    if (isset($_GET['step4']) and $_GET['step4'] == 'Yes') {
        print_step($_GET['step4_name'], $_GET['step4_temp'], $_GET['step4_duration'], $file);
    }
    if (isset($_GET['step5']) and $_GET['step5'] == 'Yes') {
        print_step($_GET['step5_name'], $_GET['step5_temp'], $_GET['step5_duration'], $file);
    }
    if (isset($_GET['step6']) and $_GET['step6'] == 'Yes') {
        print_step($_GET['step6_name'], $_GET['step6_temp'], $_GET['step6_duration'], $file);
    }
    fclose($file);



    if (isset($_GET['function']) and $_GET['function'] == 'start'){
        start_maischcontroller($path.$logfile);
        echo "started maischcontroller binary!<br>";

    }
    

}



if (isset($_GET['function']) and $_GET['function'] == 'kill'){
    exec('killall maischcontroller');
    echo "killed the program<br>";
}


//to execute as another user: sudo -H -u otheruser bash -c 'echo "I am $USER, with uid $UID"' 

//print_get_request();

?>

</body>
</html>
