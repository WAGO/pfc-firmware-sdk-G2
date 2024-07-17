<?php
//----------------------------------------------------------------------------------------------------------------------
// Copyright 2023 WAGO GmbH & Co. KG
//
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//----------------------------------------------------------------------------------------------------------------------

// Highly recommended to enable SSL verification in productive environments!
const SSL_VERIFY_HOST = false;
const SSL_VERIFY_PEER = false;

function base64url_encode($data) {
  return rtrim(strtr(base64_encode($data), '+/', '-_'), '=');
}

class HttpResult{
   public $code;
   public $type;
   public $data;
}

function post_data($uri, $headers, $content) {
  return set_data($uri, 'POST', $headers, $content);
}

function patch_data($uri, $headers, $content) {
  return set_data($uri, 'PATCH', $headers, $content);
}

function set_data($uri, $method, $headers, $content) {
  // This example uses PHP’s cURL extension:
  // https://www.php.net/manual/en/intro.curl.php

  $curl_handle = curl_init();
  curl_setopt($curl_handle, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($curl_handle, CURLOPT_URL,            $uri);
  curl_setopt($curl_handle, CURLOPT_CUSTOMREQUEST,  $method);
  curl_setopt($curl_handle, CURLOPT_FOLLOWLOCATION, true); // Follow redirects (e.g. HTTP 301)
  curl_setopt($curl_handle, CURLOPT_SSL_VERIFYHOST, SSL_VERIFY_HOST);
  curl_setopt($curl_handle, CURLOPT_SSL_VERIFYPEER, SSL_VERIFY_PEER);
  curl_setopt($curl_handle, CURLOPT_HTTPHEADER,     $headers);
  curl_setopt($curl_handle, CURLOPT_POSTFIELDS,     $content);

  $result       = new HttpResult();
  $result->data = curl_exec($curl_handle);
  $result->code = curl_getinfo($curl_handle, CURLINFO_RESPONSE_CODE);
  $result->type = curl_getinfo($curl_handle, CURLINFO_CONTENT_TYPE);

  curl_close($curl_handle);

  return $result;
}

function get_data($uri, $headers) {
  // This example uses PHP’s cURL extension:
  // https://www.php.net/manual/en/intro.curl.php

  $curl_handle = curl_init();
  curl_setopt($curl_handle, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($curl_handle, CURLOPT_URL,            $uri);
  curl_setopt($curl_handle, CURLOPT_FOLLOWLOCATION, true); // Follow redirects (e.g. HTTP 301)
  curl_setopt($curl_handle, CURLOPT_SSL_VERIFYHOST, SSL_VERIFY_HOST);
  curl_setopt($curl_handle, CURLOPT_SSL_VERIFYPEER, SSL_VERIFY_PEER);
  curl_setopt($curl_handle, CURLOPT_HTTPHEADER,     $headers);

  $result       = new HttpResult();
  $result->data = curl_exec($curl_handle);
  $result->code = curl_getinfo($curl_handle, CURLINFO_RESPONSE_CODE);
  $result->type = curl_getinfo($curl_handle, CURLINFO_CONTENT_TYPE);

  curl_close($curl_handle);

  return $result;
}

function perform_wda_login() {

  $uri             = 'https://localhost/wda';
  $basic_auth      = base64_encode($_POST['username'] . ':' . $_POST['password']);
  $request_headers = array('Authorization:' . ' Basic ' . $basic_auth);

  // This example uses PHP’s cURL extension:
  // https://www.php.net/manual/en/intro.curl.php

  $curl_handle = curl_init();
  curl_setopt($curl_handle, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($curl_handle, CURLOPT_URL,            $uri);
  curl_setopt($curl_handle, CURLOPT_FOLLOWLOCATION, true); // Follow redirects (e.g. HTTP 301)
  curl_setopt($curl_handle, CURLOPT_SSL_VERIFYHOST, SSL_VERIFY_HOST);
  curl_setopt($curl_handle, CURLOPT_SSL_VERIFYPEER, SSL_VERIFY_PEER);
  curl_setopt($curl_handle, CURLOPT_HTTPHEADER,     $request_headers);

  $wdx_token = false;
  // this function is called by curl for each header received
  curl_setopt($curl_handle, CURLOPT_HEADERFUNCTION,
    function($curl, $header) USE (&$wdx_token)
    {
      $len = strlen($header);
      $header = explode(':', $header, 2);
      if (count($header) < 2) {// ignore invalid headers
        return $len;
      }

      $header_name  = strtolower(trim($header[0]));
      $header_value = trim($header[1]);
      if ($header_name == 'wago-wdx-auth-token') {
        $wdx_token = $header_value;
      }

      return $len;
    }
  );
  curl_exec($curl_handle);
  $response_code = curl_getinfo($curl_handle, CURLINFO_RESPONSE_CODE);
  curl_close($curl_handle);

  // Get a token in exchange for the authorization code  
  if($response_code != 200) {
    $_SESSION['app_state']     = 'error';
    $_SESSION['error_message'] = 'Failed to obtain WDx token: '
                               . 'Negative result from WDA request (' . $result->code . ')';
  }
  else if($wdx_token == false) {
    $_SESSION['app_state']     = 'error';
    $_SESSION['error_message'] = 'Failed to obtain WDx token: '
                               . 'No WDx token found in WDA response (' . $result->code . ')';
  }
  else {
    $_SESSION['access_token'] = $wdx_token;
    $_SESSION['app_state']    = 'authenticated';
  }
}

function perform_login($client_id) {
  // Documentation for authentication via login page:
  // /openapi/auth.openapi.html#operation/ShowLoginPage

  // Build random code verifier
  $random_bytes   = bin2hex(openssl_random_pseudo_bytes(32));
  $code_verifier  = base64url_encode($random_bytes);
  $code_challenge = base64url_encode(hash('sha256', $code_verifier, true));

  // Store code verifier to check against in the next step (else statement below)
  $_SESSION['code_verifier'] = $code_verifier;

  $scope       = 'wda';   // The requested scope(s) depends on the services that should be used
  $auth_type   = 'code';
  $code_method = 'S256';
  $redirect_location = '/auth/authorize' . '?client_id='             . $client_id
                                         . '&scope='                 . $scope
                                         . '&response_type='         . $auth_type
                                         . '&code_challenge_method=' . $code_method
                                         . '&code_challenge='        . $code_challenge;
  header("Location: $redirect_location");
  exit();
}

function obtain_access_token($client_id) {
  // Documentation for obtaining an access token from auth service:
  // /openapi/auth.openapi.html#operation/ObtainAccessToken

  $authorization_code = $_GET['code'];
  $code_verifier      = $_SESSION['code_verifier'];

  // Code verifier is a one-time security value, drop it to avoid any re-use
  unset($_SESSION['code_verifier']);

  // Get a token in exchange for the authorization code
  $uri  = 'https://localhost/auth/token';
  $body = array(
    'grant_type'    => 'authorization_code',
    'code'          => $authorization_code,
    'client_id'     => $client_id,
    'code_verifier' => $code_verifier
  );
  $content = http_build_query($body);
  $result  = post_data($uri, array('Content-Type: ' . 'application/x-www-form-urlencoded'), $content);
  if($result->code != 200) {
    $_SESSION['app_state']     = 'error';
    $_SESSION['error_message'] = 'Failed to obtain access token: '
                               . 'Negative result from auth service token endpoint (' . $result->code . ')';
  }
  else if(    (strtolower($result->type) != 'application/json;charset=utf-8')
           && (strtolower($result->type) != 'application/json')) {
    $_SESSION['app_state']     = 'error';
    $_SESSION['error_message'] = 'Failed to obtain access token: '
                               . 'Unexpected content type in result (' . $result->type . ')';
  }
  else {
    $json_result = json_decode($result->data, true);
    if(!isset($json_result['access_token'])) {
      $_SESSION['app_state']     = 'error';
      $_SESSION['error_message'] = 'Failed to obtain access token: Found no access token in result';
    }
    else {
      $_SESSION['access_token'] = $json_result['access_token'];
      $_SESSION['app_state']    = 'authenticated';
    }
  }
}

function perform_logout() {
  // This logout follows PHP’s example to destroy a session:
  // https://www.php.net/manual/en/function.session-destroy.php

  // Unset all of the session variables
  $_SESSION = array();

  // To kill the session, also delete the session cookie
  if(ini_get("session.use_cookies")) {
    $params = session_get_cookie_params();
    setcookie(session_name(), '', time() - 42000,
      $params["path"], $params["domain"],
      $params["secure"], $params["httponly"]
    );
  }

  // Finally, destroy the session
  session_destroy();

  // Reload application
  header("Location: $_SERVER[PHP_SELF]");
  exit();
}

function wda_test_feature($device_id, $feature_name) {
  // Documentation for checking feature availability on device:
  // /openapi/wda.openapi.html#tag/Devices/operation/getFeatureList

  // Set the page limit to a value big enough to receive whole feature list on a single page
  $query  = 'page[limit]=1024';
  $uri    = 'https://localhost/wda/devices/' . $device_id . '/features' . '?' . $query;
  $result = get_data($uri, array('Authorization: Bearer ' . $_SESSION['access_token']));
  if ($result->code == 401) {
    $_SESSION['app_state'] = 'error';
    $_SESSION['error_message'] = 'Access token has expired. Refer to the '
                               . 'WDA REST-API or Auth Service API documentation '
                               . 'for information on access token renewal.';
    // Refer to /openapi/wda.openapi.html#section/Authentication
    // resp. /openapi/auth.openapi.html#operation/ObtainAccessToken
  }
  if($result->code != 200) {
    $detail_message = wda_extract_error_information('Negative result from WDA service', $result);
    $_SESSION['feature_state']         = 'error';
    $_SESSION['feature_error_message'] = 'Failed to read device feature list: '
                                       . $detail_message . ' (' . $result->code . ')';
  }
  else {
    $json_result = json_decode($result->data, true);
    if(!isset($json_result['data'])) {
      $_SESSION['feature_state']         = 'error';
      $_SESSION['feature_error_message'] = 'Failed to read device feature list: Unexpected content';
    }
    else {
      $_SESSION['feature_state'] = 'unavailable';
      foreach($json_result['data'] as $feature) {
        if($feature['attributes']['name'] == $feature_name) {
          $_SESSION['feature_state'] = 'available';
        }
      }
    }
  }
}

function wda_get_parameter_value($device_id, $parameter_path) {
  // Documentation for working with parameter values:
  // /openapi/wda.openapi.html#tag/Parameters
  $value = '';

  // Construct query to filter for wanted parameter
  $query = 'filter[device]=' . $device_id . '&filter[path]=' . $parameter_path;

  // Sending the request
  $uri    = 'https://localhost/wda/parameters' . '?' . $query;
  $result = get_data($uri, array('Authorization: Bearer ' . $_SESSION['access_token']));
  if ($result->code == 401) {
    $_SESSION['app_state'] = 'error';
    $_SESSION['error_message'] = 'Access token has expired. Refer to the '
                               . 'WDA REST-API or Auth Service API documentation '
                               . 'for information on access token renewal.';
    // Refer to /openapi/wda.openapi.html#section/Authentication
    // resp. /openapi/auth.openapi.html#operation/ObtainAccessToken
  }
  if($result->code != 200) {
    $detail_message = wda_extract_error_information('Negative result from WDA service', $result);
    $_SESSION['hostname_get_request_state'] = 'error';
    $_SESSION['hostname_get_error_message'] = 'Failed to read parameter value: '
                                            . $detail_message . ' (' . $result->code . ')';
  }
  else {
    $json_result = json_decode($result->data, true);
    if(!isset($json_result['data'])) {
      $_SESSION['hostname_get_request_state'] = 'error';
      $_SESSION['hostname_get_error_message'] = 'Failed to read parameter value: Unexpected content';
    }
    if(!isset($json_result['data'][0])) {
      $_SESSION['hostname_get_request_state'] = 'error';
      $_SESSION['hostname_get_error_message'] = 'Failed to read parameter value: Parameter not found';
    }
    else {
      $_SESSION['hostname_get_request_state'] = 'success';
      $value = $json_result['data'][0]['attributes']['value'];

      // Save parameter ID for further use
      $_SESSION['hostname_param_id'] = $json_result['data'][0]['id'];
    }
  }

  return $value;
}

function wda_set_parameter_value($device_id, $parameter_path, $new_value) {
  // Documentation for working with parameter values:
  // /openapi/wda.openapi.html#tag/Parameters

  // Read parameter to discover current parameter ID if not already done
  if(!isset($_SESSION['hostname_param_id'])) {
    wda_get_parameter_value($device_id, $parameter_path);
  }
  if(!isset($_SESSION['hostname_param_id'])) {
    $_SESSION['hostname_set_request_state'] = 'error';
    $_SESSION['hostname_set_error_message'] = 'Failed to discover parameter ID';
  }
  else {
    // Sending the request
    $uri     = 'https://localhost/wda/parameters/' . $_SESSION['hostname_param_id'];
    $headers = array('Authorization: Bearer ' . $_SESSION['access_token'],
                     'Content-Type: '         . 'application/vnd.api+json');
    $content = "{\"data\":{\"type\":\"parameters\",\"attributes\":{\"value\":\"$new_value\"}}}";
    $result  = patch_data($uri, $headers, $content);
    if ($result->code == 401) {
      $_SESSION['app_state'] = 'error';
      $_SESSION['error_message'] = 'Access token has expired. Refer to the '
                                 . 'WDA REST-API or Auth Service API documentation '
                                 . 'for information on access token renewal.';
      // Refer to /openapi/wda.openapi.html#section/Authentication
      // resp. /openapi/auth.openapi.html#operation/ObtainAccessToken
    }
    // You may only test for 20x, if you are not interested in details
    if(($result->code != 202) && ($result->code != 204)) {
      $detail_message = wda_extract_error_information('Negative result from WDA service', $result);
      $_SESSION['hostname_set_request_state'] = 'error';
      $_SESSION['hostname_set_error_message'] = 'Failed to write parameter value: '
                                              . $detail_message . ' (' . $result->code . ')';
    }
    else {
      $_SESSION['hostname_set_request_state'] = 'success';
    }
  }
}

function wda_extract_error_information($message_prefix, $result) {
  $detail_message = $message_prefix;
  if(strtolower($result->type) == 'application/vnd.api+json') {
    $json_result = json_decode($result->data, true);
    if(isset($json_result['errors'])) {
      $detail_message = $detail_message . ': ' . $json_result['errors'][0]['title'];
      // Errors may also contain a detailed developer message for debug purposes ($json_result['errors'][0]['detail'])
      // Errors may also contain an WDA code to dig deeper into WDA error cause ($json_result['errors'][0]['code'])
      // WAGO support staff may ask for this code to allow further problem analysis
    }
  }

  return $detail_message;
}

session_name('wda_client');
session_start();

// Check application/login state
// Possible states:
// - 'unauthenticated'
// - 'authenticated'
// - 'error'
if(!isset($_SESSION['app_state'])) {
  $_SESSION['app_state'] = 'unauthenticated';
}

if($_SESSION['app_state'] != 'authenticated') {

  // The client id used to identify this demo application as 
  // configured in /etc/authd/authd.conf
  $client_id = 'wdaphpdemo';

  // User clicked on "Login via WDA"
  if(isset($_POST['state_change']) && ($_POST['state_change'] == 'wda_login')) {
    perform_wda_login();
  }
  // User clicked on "Login via Auth Service"
  if(isset($_POST['state_change']) && ($_POST['state_change'] == 'login')) {
    perform_login($client_id);
  }

  // User was redirected after successful login via auth service
  else if(isset($_GET['code'])) {
    obtain_access_token($client_id);
  }

  // User was redirected after canceled or negative validated (e.g. unauthorized) login via auth service
  else if(isset($_GET['error'])) {
    $detail_message = '';
    if(isset($_GET['error_description'])) {
      $detail_message = $_GET['error_description'] . ' ';
    }
    $_SESSION['app_state']     = 'error';
    $_SESSION['error_message'] = 'Failed to authenticate/authorize: ' . $detail_message . '(' . $_GET['error'] . ')';
  }
}

if($_SESSION['app_state'] == 'authenticated') {
  if(isset($_POST['state_change']) && ($_POST['state_change'] == 'logout')) {
    perform_logout();
  }

  $device_id    = '0-0';
  $feature_name = 'Network';
  $param_path   = 'Networking/Hostname/CustomName';
  wda_test_feature($device_id, $feature_name);
  if($_SESSION['feature_state'] == 'available') {
    // Write new value if requested
    if(isset($_POST['new_hostname'])) {
      wda_set_parameter_value($device_id, $param_path, $_POST['new_hostname']);
    }

    // Read current value
    $_SESSION['current_hostname'] = wda_get_parameter_value($device_id, $param_path);
  }
}

if($_SESSION['app_state'] == 'error') {
  // Some custom error handling
}
?>

<!DOCTYPE html>
<html lang="de">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP WDA demo</title>
    <link rel="stylesheet" href="style.css">
  </head>
  <body>
    <div class="viewport">
      <div class="header">
        <img class="logo" draggable="false" src="wago-logo.svg" alt="logo">
      </div>
      <div class="content">
        <div class="login-state">
          <label>Application state:</label>
          <input class="value" value="<?php echo $_SESSION['app_state']; ?>" readonly>
          <div class="info">
            Possible values for application state:
            <ul>
              <li>unauthenticated</li>
              <li>authenticated</li>
              <li>error</li>
            </ul>
          </div>
        </div>
        <?php if($_SESSION['app_state'] == 'error') {
          echo '<div class="error-message">
                  <label>Error message:</label>
                  <div class="value">' . $_SESSION['error_message'] . '</div>
                </div>';
        } ?>

        <?php if($_SESSION['app_state'] != 'authenticated') {
          echo '<form class="login-redirect" action="" method="post">
                  <p>Use the Auth Service to login (recommended, available in Firmware 27 (v04.05.xx) and newer)</p>
                  <input name="state_change" id="state_change" type="hidden" value="login" readonly>
                  <button type="submit">Login via OAuth 2.0</button>
                  <p>
                    For more information about authentication and authorization see Auth-Service API documentation:<br>
                    <a href="/openapi/auth.openapi.html">openapi/auth.openapi.html</a>
                  </p>
                </form>
                <form class="login-wda" action="" method="post">
                  <p>--- OR ---</p>
                  <p>Directly authenticate against WDA</p>
                  <input name="state_change" id="state_change" type="hidden" value="wda_login" readonly>
                  <input name="username" id="username" type="text" placeholder="username">
                  <input name="password" id="password" type="password" placeholder="password">
                  <br>
                  <button type="submit">Login via WDA</button>
                </form>';
        }
        else {
          echo '<form class="logout" action="" method="post">
                  <input name="state_change" id="state_change" type="hidden" value="logout" readonly>

                  <button type="submit">Logout</button>
                </form>';
        } ?>
      </div>
      <div class="content">
        <?php if($_SESSION['app_state'] != 'authenticated') {
          echo '<p>Login as "admin" to tryout WDA demo functionality.</p>';
        }
        else {
          echo '<div class="feature-state">
            <label>Hostname related feature "Network":</label>
            <input class="value" value="' . $_SESSION['feature_state'] . '" readonly>
            <div class="info">
              Possible values for feature test:
              <ul>
                <li>unavailable</li>
                <li>available</li>
                <li>error</li>
              </ul>
            </div>
          </div>';
        } if(isset($_SESSION['feature_state']) && ($_SESSION['feature_state'] == 'error')) {
          echo '<div class="error-message">
                  <label>Error message:</label>
                  <div class="value">' . $_SESSION['feature_error_message'] . '</div>
                </div>';
        } ?>

        <?php if(    (     $_SESSION['app_state'] == 'authenticated')
                  && isset($_SESSION['feature_state'])
                  && (     $_SESSION['feature_state'] == 'available')
                  && isset($_SESSION['hostname_get_request_state'])
                  && (     $_SESSION['hostname_get_request_state'] == 'success')) {
        echo '<div class="current-hostname">
                <label>Current custom hostname:</label>
                <input class="value" value="' . $_SESSION['current_hostname'] . '" readonly>
              </div>

              <form class="next-hostname" action="" method="post">
                <label for="hostname">New custom hostname:</label>
                <input name="new_hostname" id="new_hostname" type="text">

                <button type="submit">Write new value</button>
              </form>';
        } ?>
        <?php if(    isset($_SESSION['hostname_get_request_state'])
                  && ($_SESSION['hostname_get_request_state'] == 'error')) {
          echo '<div class="error-message">
                  <label>Error message:</label>
                  <div class="value">' . $_SESSION['hostname_get_error_message'] . '</div>
                </div>';
        } ?>
        <?php if(    isset($_SESSION['hostname_set_request_state'])
                  && ($_SESSION['hostname_set_request_state'] == 'error')) {
          echo '<div class="error-message">
                  <label>Error message:</label>
                  <div class="value">' . $_SESSION['hostname_set_error_message'] . '</div>
                </div>';
        } ?>

        <p>
          For more information about device parametrization see WDA REST-API documentation:<br>
          <a href="/openapi/wda.openapi.html">openapi/wda.openapi.html</a>
        </p>
      </div>
      <div class="content source-code">
        <p>This page was created from the following PHP source code:</p>
        <div class="textarea"><?php echo htmlspecialchars(file_get_contents(__FILE__)); ?></div>
      </div>
      <div class="footer">
        <span class="app-info">WDA PHP Demo Application - Version 1.0.0</span>
        <span class="copyright-info">Copyright © 2023 WAGO GmbH &amp; Co. KG</span>
      </div>
    </div>
  </body>
</html>
