<?php
include 'mashcontrol_inc.php';

print_header();


if (isset($_GET['function']) and $_GET['function'] == 'kill'){
    exec('killall mashcontrol');
    echo "killed the program<br>";
}

if (isset($_GET['function']) and $_GET['function'] == 'continue'){
    send_sigusr1_signal();
}


if(!is_running() and !isset($_GET['recipe_name'])) {                 
    //first page (landing page)
    draw_recipe_form();
} elseif (!is_running() and isset($_GET['recipe_name'])) {           
    //second page, recipe info available, start binary
    $recipe_name = $_GET['recipe_name'];   
    $date = date('Y-m-d');
    $filename = $date."_".cleanup_recipe_name($recipe_name).".msf";
    $logfile = $date."_".cleanup_recipe_name($recipe_name)."";
    //echo "filename is ".$filename."<br>";
    //echo "full path is ".$path.$filename."<br>";
    
    write_msf_file($filename, $_GET);
    
    if (isset($_GET['function']) and $_GET['function'] == 'start'){
        start_mashcontrol($path.$logfile);
        echo "started Mashcontrol binary!<br>\n";
    }
    echo "Please wait! You will be forwarded to the next page within 10 seconds...<br>\n";
} elseif (is_running()) {           
    //third page, binary is currently running.
    draw_kill_button();
    draw_continue_button();
    
    //determine most recently modified file ending with ".log". store filename in $latest_filename
    $latest_ctime = 0;
    $latest_filename = '';    
    $d = dir($path);
    while (false !== ($entry = $d->read())) {
        $filepath = "{$path}/{$entry}";
        $len = strlen($filepath);
        // could do also other checks than just checking whether the entry is a file
        if (is_file($filepath) && filectime($filepath) > $latest_ctime && (substr($filepath, $len-4) == ".log") ) {
            $latest_ctime = filectime($filepath);
            $latest_filename = $entry;
        }
    }
    get_current_status($latest_filename);    
    
    //do not use the above. 
    $latest_filename = "mashcontrol_console_output.txt";
    get_current_status($latest_filename);
} 






//to execute as another user: sudo -H -u otheruser bash -c 'echo "I am $USER, with uid $UID"' 

//print_get_request();

?>

</body>
</html>
