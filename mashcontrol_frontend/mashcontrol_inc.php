<?php


$path = "/home/pi/mashcontrol/";    //path for mash step file



function get_current_status($logfile)
{
    exec("tail $logfile.log", $output);
    echo "<hr>";
    echo "printing last ten lines from $logfile.log: <pre>";
    print_r($output);
    echo "</pre>";
    echo "<hr>";
    
    $output = "";
    exec("tail /home/pi/mashcontrol/mashcontrol_console_output.txt", $output);
    echo "<hr>";
    echo "printing last ten lines from mashcontrol_console_output.txt:<pre>";
    print_r($output);
    echo "</pre>";
    echo "<hr>";
    
}

function start_mashcontrol($mashcontrol_logfile)
{
//  /home/pi/mashcontrol/mashcontrol /home/pi/mashcontrol/2018-03-06_test06 > /home/pi/mashcontrol/mashcontrol_console_output.txt 2>&1 & echo $! > /home/pi/mashcontrol/pidfile.txt

    global $path;
    $cmd = '/home/pi/mashcontrol/mashcontrol';
    $outputfile = '/home/pi/mashcontrol/mashcontrol_console_output.txt';
    $pidfile = $path.'pidfile.txt';
    
      $full_command = sprintf("%s %s > %s 2>&1 & echo $! > %s", $cmd, $mashcontrol_logfile, $outputfile, $pidfile);
    //$full_command = sprintf("%s %s > %s 2>&1", $cmd, $mashcontrol_logfile, $outputfile);
    //from https://stackoverflow.com/questions/45953/php-execute-a-background-process, 
    //$! is the PID of the most recent background command
    exec($full_command);   

    echo "<pre>";
    echo $full_command;
    echo "</pre>";
}


function send_sigusr1_signal()
{
    global $path;
    $file = fopen($path."pidfile.txt", "r");
    if ($file == false) {
        echo "Error opening file pidfile.txt! Exiting!";
        exit();
    }
    $pid = fread($file, filesize($path."pidfile.txt"));
    fclose($file);
    $cmd = "kill -USR1 $pid";
    echo "<pre>";
    echo $cmd;
    echo "</pre>";
    exec($cmd);
}

function print_step($name, $temp, $duration, $file)
{
    //echo $name.", ".$temp.", ".$duration."<br>";
    fwrite($file, $name.", ".$temp.", ".$duration."\n");
}

function print_get_request()
{
    echo "<hr><hr>";
    echo "<pre>";
    print_r($_GET);
    echo "</pre>";
}

function cleanup_recipe_name($name)
{
    $name = str_replace(" ", "_", $name);   // change space to underscore
    $name = str_replace("ä", "ae", $name);  // change ä to ae
    $name = str_replace("Ä", "Ae", $name);  // change Ä to Ae
    $name = str_replace("ö", "oe", $name);  // change ö to oe
    $name = str_replace("Ö", "Oe", $name);  // change Ö to Oe
    $name = str_replace("Ü", "ue", $name);  // change ü to ue
    $name = str_replace("Ü", "Ue", $name);  // change Ü to Ue
    return $name;
}

?>
