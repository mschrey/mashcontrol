<?php


$path = "/home/pi/mashcontrol/";    //path for mash step file

function print_header()
{
    if ((is_running()) or (!is_running() and isset($_GET['recipe_name']))) {    
        header("Refresh:10; url=mashcontrol.php");
    }
    echo "<!DOCTYPE html>\n";
    echo "<head>\n";
    echo "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n";
    echo "</head>\n";
    echo "\n";
    echo "<body>\n";
    echo "<h3>Mashcontrol - beta</h3>\n";
};

function get_current_status($logfile)
{
    global $path;
    exec("tail $path$logfile", $output);
//    echo "<hr>";
//    echo "Status from $path$logfile: \n";
//    echo "<pre>\n";    
    foreach($output as $i => $string) {
        printf("<!--%d => %s-->\n", $i, htmlspecialchars($string));
    }
//    echo "</pre>\n";
//    echo "<hr>\n\n";   
    return $output[9];
}

function start_mashcontrol($mashcontrol_logfile)
{
//stdbuf solution for buffering issue taken from https://stackoverflow.com/questions/1429951/force-flushing-of-output-to-a-file-while-bash-script-is-still-running
//  stdbuf -oL /home/pi/mashcontrol/mashcontrol /home/pi/mashcontrol/2018-03-06_test06 > /home/pi/mashcontrol/mashcontrol_console_output.txt 2>&1 & echo $! > /home/pi/mashcontrol/pidfile.txt

    global $path;
    $cmd = '/home/pi/mashcontrol/mashcontrol';
    $outputfile = '/home/pi/mashcontrol/mashcontrol_console_output.txt';
    $pidfile = $path.'pidfile.txt';
    
      $full_command = sprintf("stdbuf -oL %s %s > %s 2>&1 & echo $! > %s", $cmd, $mashcontrol_logfile, $outputfile, $pidfile);
    //$full_command = sprintf("%s %s > %s 2>&1", $cmd, $mashcontrol_logfile, $outputfile);
    //from https://stackoverflow.com/questions/45953/php-execute-a-background-process, 
    //$! is the PID of the most recent background command
    exec($full_command);   
    sleep(1);
    
    echo "<pre>";
    echo $full_command;
    echo "</pre>";
}


function write_msf_file($filename, $myGET)
{
    global $path;
    $file = fopen($path.$filename, "w");   //this writes to /home/pi/mashcontrol and therefore requires chmod 777 /home/pi/mashcontrol...  :(
    $date = date('Y-m-d');
    if ($file == false) {
        echo "Error opening file! Exiting!";
        exit();
    }
    
    $recipe_name = $myGET['recipe_name'];  
    fwrite($file, "# Mash programm for ".$recipe_name."\n");
    fwrite($file, "# created by Mashcontrol web frontend on ".$date."\n");
    if ($myGET['step1'] == 'Yes') {
        print_step($myGET['step1_name'], $myGET['step1_temp'], $myGET['step1_duration'], $file);
    }
    if (isset($myGET['step2']) and $myGET['step2'] == 'Yes') {
        print_step($myGET['step2_name'], $myGET['step2_temp'], $myGET['step2_duration'], $file);
    }
    if (isset($myGET['step3']) and $myGET['step3'] == 'Yes') {
        print_step($myGET['step3_name'], $myGET['step3_temp'], $myGET['step3_duration'], $file);
    }
    if (isset($myGET['step4']) and $myGET['step4'] == 'Yes') {
        print_step($myGET['step4_name'], $myGET['step4_temp'], $myGET['step4_duration'], $file);
    }
    if (isset($myGET['step5']) and $myGET['step5'] == 'Yes') {
        print_step($myGET['step5_name'], $myGET['step5_temp'], $myGET['step5_duration'], $file);
    }
    if (isset($myGET['step6']) and $myGET['step6'] == 'Yes') {
        print_step($myGET['step6_name'], $myGET['step6_temp'], $myGET['step6_duration'], $file);
    }
    fclose($file);

}


function get_pid()
{
    global $path;
    if(!(file_exists($path."pidfile.txt"))) {
        return 0;   //pidfile.txt does not exist. mashcontrol binary was never executed on this computer
    }
    $file = fopen($path."pidfile.txt", "r");
    if ($file == false) {
        echo "Error opening file pidfile.txt! Exiting!";
        exit();
    }
    $pid = fread($file, filesize($path."pidfile.txt"));
    fclose($file);
    $pid = rtrim($pid);
    
    return $pid;
}



function send_sigusr1_signal()
{
    $pid = get_pid();
    $cmd = "kill -USR1 $pid";
    //echo "<pre>";
    //echo $cmd;
    //echo "</pre>";
    exec($cmd);
}


function is_running()
{
    $pid = get_pid();
    if ($pid != 0) {
        $res = file_exists("/proc/$pid/cmdline");
        return $res;
    } else {
        return false;
    }
    //if($res) {
    //    echo "mashcontrol is running<br>";
    //} else {
    //    echo "mashcontrol is not running<br>";
    // }
    return $res;
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


function getRastnameFromConsole($lastline)
{
    $array = explode(",", $lastline);
    $array1 = explode(" ", $array[0]);    
    $rastName = $array1[2];
    return $rastName;
}

function getTempShouldFromConsole($lastline)
{
    $array = explode(",", $lastline);
    $array1 = explode(": ", $array[1]);
    $temp_should = floatval($array1[1]);
    return $temp_should;
}

function getTempIsFromConsole($lastline)
{
    $array = explode(",", $lastline);
    $array1 = explode(": ", $array[2]);
    $temp_is = floatval($array1[1]);
    return $temp_is;
}

function getHeaterStatusFromConsole($lastline)
{
    $array = explode(",", $lastline);
    $array1 = explode(": ", $array[3]);
    $heater_status = $array1[1];
    return $heater_status;
}

function getRastDurationTotalFromConsole($lastline)
{
    $array = explode(",", $lastline);
    $array1 = preg_split("/[\s]+/", $array[4]);  //use regex for second parsing due to possibility of two spaces
    $rastDurationTotal = substr($array1[4], 0, 2);
    return $rastDurationTotal;
}

function getRastDurationRemainingFromConsole($lastline)
{
    $array = explode(",", $lastline);
    $array1 = preg_split("/[\s]+/", $array[4]);  //use regex for second parsing due to possibility of two spaces
    $rastDurationRemaining = $array1[2];
    return $rastDurationRemaining;
}


function newTableCell()
{
    printf("  </td><td width=\"25%%\" align=\"center\">\n");        
}

function parse_console_output($lastline) 
{
    $currentDate = date('Y-m-d');
    if(strcmp(substr($lastline, 0, strlen($currentDate)),$currentDate) == 0) {
        printf("<table width=\"100%%\" border=\"3\"><tr>\n");
        
        newTableCell();
        printf("  <img src=\"questionmark.png\" height=\"170\"><br>\n");
        printf("  Rastname:<br><font size=\"5\"><b>%s</b></font>\n", getRastnameFromConsole($lastline));
        
        newTableCell();
        printf("  <img src=\"thermometer1.php?current=%d&max=%d\" height=\"170\" width=\"54\" alt=\"current temp, daily min/max\" title=\"current temp daily, min/max\" /><br>\n", round(getTempIsFromConsole($lastline)), getTempShouldFromConsole($lastline));
        printf("  Soll: <font size=\"5\"><b>%s°C</b></font><br>\n", getTempShouldFromConsole($lastline));   
        printf("  Ist: <font size=\"5\"><b>%s°C</b></font>\n", getTempIsFromConsole($lastline));   
        
        newTableCell();
        if(strpos($lastline, " Noch ") !== false) {
            printf("  <img src=\"hourglass.png\" height=\"170\"><br>\n");
            printf("  Rast ongoing. <br><font size=\"5\"><b>%s of %smin</b></font><br> remaining\n", getRastDurationRemainingFromConsole($lastline), getRastDurationTotalfromConsole($lastline));                
        } elseif (strpos($lastline, "Temperature reached") !== false) {
            draw_continue_button();    
            printf("  user must click \"Continue\"!<br>\n");            
        }        
        
        newTableCell();
        if(strcmp(getHeaterStatusFromConsole($lastline), "OFF") == 0) {
            printf("  <img src=\"heater-off.png\" height=\"170\"><br>\n");
        } else {
            printf("  <img src=\"heater-on.png\"  height=\"170\"><br>\n");
        }    
        printf("  heater status: <br><font size=\"5\"><b>%s</b></font>\n", getHeaterStatusFromConsole($lastline));   
                
        printf("  </td>\n");                
        printf("  </tr>\n</table>\n");
    }
    
    draw_kill_button();

    
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


function draw_continue_button()
{
//    printf("<form action=\"mashcontrol.php\" method=\"get\">\n");
//    printf("<input type=\"hidden\" name=\"function\" value=\"continue\">\n");
//    printf("<input type=\"submit\" value=\"Continue\">\n");
//    printf("</form>\n");
    printf("<a href=\"mashcontrol.php?function=continue\"><img src=\"circular-arrow-leave.png\" height=\"170\"></a><br>\n");
}


function draw_kill_button()
{
    printf("<form action=\"mashcontrol.php\" method=\"get\">\n");
    printf("<input type=\"hidden\" name=\"function\" value=\"kill\">\n");
    //printf("<input type=\"hidden\" name=\"logfile\" value=%s>\n", $logfile);
    printf("<input type=\"submit\" value=\"Kill Mashcontrol\">\n");
    printf("</form>\n");
}




function draw_recipe_form()
{
    $rastname_array = array("MashIn", "Eiweissrast", "Maltoserast", "Kombirast", "Verzuckerungsrast", "Mashout");
    $rastenabled_array = array(1, 0, 1, 0, 1, 1);              //default value: 1 = rast is enabled; 0 = rast is disabled
    $rasttemp_array = array(60, 57, 63, 68, 72, 78);           //default temperature for each rast
    $rastdurationchoice_array = array(0, 20, 35, 60, 20, 0);   //default duration for each rast, if this is 0 then no duration can be chosen (e.g. Mashin)

    $rastduration_array = array(10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90); //this lists the possible duration values for every rast

    //recipe name
    printf("<form action=\"mashcontrol.php\" method=\"get\" >\n");
    printf("<p>Recipe Name: <input type=\"text\" name=\"recipe_name\"></p>\n");
    printf("\n\n");


    echo "<table border=\"0\">";
    for ($i=0; $i<=5;$i++) {
        echo "<tr><td>\n";
        printf("Rast %s:\n", $i+1);
        //input type=checkbox...
        printf("<input type=\"checkbox\" name=\"step%s\" value=\"Yes\"", $i+1);
        if($rastenabled_array[$i] == 1) {
            printf(" checked=\"checked\"> \n");
        } else {
            printf("> \n");
        }
        //input type=hidden...
        printf("<input type=\"hidden\" name=\"step%s_name\" value=\"%s\">\n", $i+1, $rastname_array[$i]);
        
        //Temperature input field...
        printf("%s: </td><td>Temperature <input type=\"text\" name=\"step%s_temp\" value=\"%s\" size=\"5\">&deg;C</td><td>\n", $rastname_array[$i], $i+1, $rasttemp_array[$i]);
        
        //Duration Dropdown Menu
        //duration can only be chosen if $rastdurationchoice_array[$i] is not 0
        if($rastdurationchoice_array[$i] != 0) {
            printf("Duration: \n");
            printf("<select name=\"step%s_duration\">", $i+1);
            foreach($rastduration_array as $duration) {
                printf("<option value=\"%s\"", $duration);
                if($duration == $rastdurationchoice_array[$i]) {
                    printf(" selected=\"selected\">%s</option>\n", $duration);
                } else {
                    printf(">%s</option>\n", $duration);
                }
            }
            printf("</select>\n");
        } else {
            printf("<input type=\"hidden\" name=\"step%s_duration\" value=\"0\">\n", $i+1);
        }
        
        printf("</td></tr>\n");
        
        printf("\n\n");
    }
    echo "</table>";

    printf("<input type=\"hidden\" name=\"function\" value=\"start\">\n");
    printf("<input type=\"submit\" value=\"Start Mashcontrol\">\n");
    printf("</form>\n\n");
}
?>
