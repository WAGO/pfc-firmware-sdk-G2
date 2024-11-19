<?php

define("SUCCESS", 0);
define("ERROR",   -1);
define("ACCESS_NOT_ALLOWED", -2);

//define("MOCK", true);
define("MOCK", false);

define("USERLEVEL_GUEST", 0);
define("USERLEVEL_USER",  2);
define("USERLEVEL_ADMIN", 3);

function MockConfigtool($callString, &$dummy, &$status)
{
  $status       = 0;
  $resultString = "";
  
  switch($callString)
  {
    // read tools
    
    case "/etc/config-tools/get_coupler_details order-number":
    
      $resultString = "0758-0874-0000-0111";
      break;
      
    case "/etc/config-tools/get_actual_eth_config eth0 ip-address":
    
      $resultString = "192.168.1.17";
      break;
      
    // write tools

    case "/etc/config-tools/change_hostname hostname=test":
    
      $resultString = "";
      break;
    
    case "/etc/config-tools/change_hostname hostname=te+st":
    
      $resultString = "";
      break;
    
    case "/etc/config-tools/config_interfaces interface=eth0 ip-address=192.168.1.18":
    
      $resultString = "";
      break;
    
    // provocated fails
      
    case "/etc/config-tools/timeout":
    
      sleep(2);
      $resultString = "output after long time";
      break;
    
    case "/etc/config-tools/empty_string":
    
       $resultString = "";
       break;
       
     case "/etc/config-tools/time":
     
       $resultString = sprintf("%d", time());
       break;
       
    case "/etc/config-tools/no_jason_return":
      
      echo "bla";
      die();
      
    case "/etc/config-tools/get_touchscreen_config display-state":
    
      $resultString = "off";
      break;
      
    default: $status = -1;
  }
  
  return $resultString;
}

function AccessAllowed($configtool, $username = "")
{
  // userlevel => configtool names
  $accessRights = array(
    
    USERLEVEL_ADMIN => array(
        'get_user_info',

        // wtp
        'config_browsersecurity'
        ),
      
    USERLEVEL_USER  => array(
        
        'config_clock',
        
        // wtp
        'get_audio',
        'get_autostart',
        'get_backlight',
        'get_boot',
        'get_browsersecurity',
        'get_configured',
        'get_display',
        'get_gesture',
        'get_motionsensor',
        'get_operatinghours',
        'get_plcselect',
        'get_screensaver',
        'get_touchbeeper',
        'start_browser',
        'start_displaycleaning',
        'start_screensaver',
        'start_touchcalib',
        'testbacklight',
        
        'config_audio',
        'config_autostart',
        'config_backlight',
        'config_boot',
        'config_browsersecurity',
        'config_configured',
        'config_display',
        'config_gesture',
        'config_motionsensor',
        'config_plcselect',
        'config_screensaver',
        'config_touchbeeper'
        ),
      
    USERLEVEL_GUEST => array(
        'get_coupler_details',
        'get_typelabel_value',
        'get_runtime_config',
        'get_clock_data',
        'get_eth_config',
        'get_actual_eth_config',
        'get_plc_config',
        'get_run_stop_switch_value',
        'get_led_config',
        'get_rts_info',
        'get_port_state',
        'get_wbm_diaglist'
        ) 
  );
  
  $accessRightsLocalhost = array(
      USERLEVEL_GUEST => array(
        'start_browser'
        )
  );
  

  $accessAllowed    = false;
  $callViaLocalhost = ("127.0.0.1" == $_SERVER["REMOTE_ADDR"]) ? true : false;
  
  // for debugging
  //$execString = "echo REMOTE_ADDR=".$_SERVER["REMOTE_ADDR"]." configtool:".$configtool." > /tmp/wbm_debug.txt";
  //exec($execString, $dummyOutput, $returnSatus);

  switch($username)
  {
    case 'user':   $userlevel = USERLEVEL_USER; break;
    case 'admin':  $userlevel = USERLEVEL_ADMIN; break;
    default:       $userlevel = USERLEVEL_GUEST; break;
  }
  
  if((USERLEVEL_GUEST <= $userlevel) && in_array($configtool, $accessRights[USERLEVEL_GUEST]))
  {
    $accessAllowed = true;
  }
  else if((USERLEVEL_USER <= $userlevel) && in_array($configtool, $accessRights[USERLEVEL_USER]))
  {
    $accessAllowed = true;
  }
  
  // access for admin allways allowed
  else if((USERLEVEL_ADMIN <= $userlevel)) // && in_array($configtool, $accessRights[USERLEVEL_ADMIN]))
  {
    $accessAllowed = true;
  }
  
  // access allowed for call via localhost without login
  else if($callViaLocalhost && in_array($configtool, $accessRightsLocalhost[USERLEVEL_GUEST]))
  {
    $accessAllowed = true;
  }
  
  // else accessAllowed is still false
  
  return $accessAllowed;
}

function ConfigtoolNameInvalid($name)
{
  $nameInvalid = false;
  
  if(strpos($name, "/")) 
  {
    $nameInvalid = true;
  }
  
  return $nameInvalid;
}

function CallConfigtool($configtoolObj, &$resultObj, $username = "")
{
  $status       = SUCCESS;
  $resultString = "";
  $errorText    = "";
  $callString   = "";
  
  if(!isset($configtoolObj))
  {
    $status     = ERROR;
    $errorText  = "Missing input";
  }
    
  else if(ConfigtoolNameInvalid($configtoolObj["name"]))
  {
    $status     = ERROR;
    $errorText  = "Invalid configtool name (".$configtoolObj["name"].")";
  }
  
  else if(!file_exists(sprintf("/etc/config-tools/%s", $configtoolObj["name"])))
  {
    $status     = ERROR;
    $errorText  = "Unknown configtool (".$configtoolObj["name"].")";
  }

  else if(!AccessAllowed($configtoolObj["name"], $username))
  {
    $status     = ACCESS_NOT_ALLOWED;
    $errorText  = "Access not allowed.";
  }

  else
  {
    if(isset($configtoolObj["sudo"]) && $configtoolObj["sudo"])
    {
      $callString = "sudo ";
    }
    
    // create string to call configtool by linux shell - first directory and configtool name
    $callString = $callString."/etc/config-tools/".$configtoolObj["name"];
    
    // now all configtool parameters, one after the other
    $paramNo = 0;
    while(isset($configtoolObj["parameter"][$paramNo]))
    {
      // decode parameter in url format (to save spaces eg.), but '=' (changed to "%3d") must be decoded again
      /*
      $paramString = urlencode($configtoolObj["parameter"][$paramNo]);
      $paramString = str_replace("%3D", "=", $paramString);
      $paramString = str_replace("%3A", ":", $paramString);
      $paramString = str_replace("%2F", "/", $paramString);
      */
      
      // decode '%' and '+', because they have special meanings in the encoded characters
      $paramString = $configtoolObj["parameter"][$paramNo];
      $paramString = str_replace("%", "%25", $paramString);
      $paramString = str_replace("+", "%2b", $paramString);
      
      // decode characters in url format, which otherwise confuse the bash
      $paramString = str_replace(" ", "%20", $paramString);
      $paramString = str_replace("'", "%27", $paramString);
      $paramString = str_replace('"', "%22", $paramString);
      $paramString = str_replace('`', "%60", $paramString);
      $paramString = str_replace('*', "%2a", $paramString);
      
      // escape parameter string for bash
      // (add (single!) quotation marks around parameter string to let the bash don't interprete "(", ")", and so on
      // and mask quotation mark itself inside string)
      // "escapeshellarg" does not work, because it strippes UTF8 characters from string and setlocale does not help
      $paramString = "'".str_replace("'", "'\\''", $paramString)."'";
      
      // add parameter to call string
      $callString = $callString." ".$paramString;

      ++$paramNo;
    }
    //printf(" callString:%s\n", $callString);
    
    $dummyOutput;

    // unit test is executed - get specified test output
    if(MOCK) $resultString = MockConfigtool($callString, $dummyOutput, $status);
    
    // multiline output expected - use special shell call, which transfers more than one output line
    else if(isset($configtoolObj["multiline"]) && $configtoolObj["multiline"])
    {
      $resultString = shell_exec($callString);
    }
    
    // normal call of configtool
    else $resultString = exec($callString, $dummyOutput, $status);

    if($resultString == NULL) $resultString = "";
    
    if(0 != $status)
    {
      $lastErrorFilename = "/tmp/last_error.txt";
      if(file_exists($lastErrorFilename))
      {
        $errorText = file_get_contents($lastErrorFilename);
        //unlink($lastErrorFilename);
        exec(sprintf("sudo /bin/rm %s", $lastErrorFilename));
      }
    }
  }
  
  // to return call string (only!) for debugging, comment out next line meanwhile
  $callString = "";
    
  $resultObj = array('status'       => $status,
                     'resultString' => $resultString,
                     'errorText'    => $errorText,
                     'dataId'       => isset($configtoolObj["dataId"]) ? $configtoolObj["dataId"] : "",
                     'callString'   => $callString );
}

function CallConfigtoolByJson()
{
  // get request json string by stdin
  $requestString    = file_get_contents("php://input");
  //$userlevel        = 0;
  $username         = "";
  
  //echo "requestString:".$requestString;

  if($requestString)
  {
    // decode string to php array/object
    $aRequest = json_decode($requestString, true);
    //var_dump($aRequest);

    // take care about compabilty for old version (without session id), just use deviceParam array then
    if(isset($aRequest["sessionId"]))
    {
      $aDeviceParams = isset($aRequest["aDeviceParams"]) ? $aRequest["aDeviceParams"] : array();

      // get authenticated username 
      if(strlen($aRequest["sessionId"]))
      { 
        session_id($aRequest["sessionId"]);
        session_start();
      }
      
      //$userlevel = isset($_SESSION["userlevel"]) ? intval($_SESSION["userlevel"]) : 0; 
      $username = isset($_SESSION["username"]) ? $_SESSION["username"] : "";
    }
    else
    {
      $aDeviceParams = $aRequest;
      $username      = "guest";
    }
    
    // loop over all configtool calls in request, collect response data in response array
    $configtoolNo = 0;
    while(isset($aDeviceParams[$configtoolNo]))
    {
      CallConfigtool($aDeviceParams[$configtoolNo], $aDeviceResponse[$configtoolNo], $username);
      ++$configtoolNo;
    }
  }

  if(isset($aRequest["sessionId"]))
  {
    $aResponse  = array("sessionId" => $aRequest["sessionId"], "aDeviceResponse" => $aDeviceResponse);
  }
  else
  {
    $aResponse = isset($aDeviceResponse) ? $aDeviceResponse : false;
  }
  
  // encode responses to json string and send it to stdout
  //var_dump($aResponse);
  echo json_encode($aResponse);
}

CallConfigtoolByJson();

?>
