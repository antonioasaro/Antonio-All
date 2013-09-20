<?PHP

$serial_num = "NULL";
$location = "Orlando,Florida";
$units = "Imperial";

foreach (getallheaders() as $name => $value) {
    if (strcmp($name, "Host") == 0) { $serial_num = "3530114C2005";  }
//    if (strcmp($name, "X-Pebble-ID") == 0) { $serial_num = $value; }
}   

$hostname = "mysql4.000webhost.com"; 
$username = "a7417365_all";
$password = "a7417365_passwd";
$dbname   = "a7417365_all"; 
$tbname   = "weather_quotes"; 

$dbhandle = mysql_connect($hostname, $username, $password) or die();
$selected = mysql_select_db("a7417365_all",$dbhandle) or die();
$result = mysql_query("SELECT * FROM ".$tbname." WHERE SERIAL_NUM = '".$serial_num."'");
while ($row = mysql_fetch_array($result)) {
   $location=$row{'LOCATION'}; $units=$row{'UNITS'};
}
mysql_close($dbhandle);


$json  = curl_get('http://api.openweathermap.org/data/2.5/weather?q='.$location.'&units='.$units);
$weather = process_weather($json);
print json_encode($weather);

function curl_get($url){
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_REFERER, $url);
    curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/1.0");
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    $output = curl_exec($ch);
    curl_close($ch);
    return $output;
}

function process_weather($json_in) {
    $json_output = json_decode(utf8_decode($json_in));
    if (!$json_output) die(); 

    $weather     = $json_output->weather;
    $description = $weather[0]->description;
    $temperature = $json_output->main->temp;
    $icon        = $weather[0]->icon;

    $result    = array();
    $result[1] = $description;
    $result[2] = array('I', round($temperature, 0));
    $result[3] = $icon;
    return $result;
}

/*
{   "id":88319,
    "dt":1345284000,
    "name":"Benghazi",
    "coord":{"lat":32.12,"lon":20.07},
    "main":{"temp":306.15,"pressure":1013,"humidity":44,"temp_min":306,"temp_max":306},
    "wind":{"speed":1,"deg":-7},
    "weather":[
                 {"id":520,"main":"Rain","description":"light intensity shower rain","icon":"09d"},
                 {"id":500,"main":"Rain","description":"light rain","icon":"10d"},
                 {"id":701,"main":"Mist","description":"mist","icon":"50d"}
              ],
    "clouds":{"all":90},
    "rain":{"3h":3}}

Day	Night	
01d.png	 01n.png	 sky is clear
02d.png	 02n.png	 few clouds
03d.png	 03n.png	 scattered clouds
04d.png	 04n.png	 broken clouds
09d.png	 09n.png	 shower rain
10d.png	 10n.png	 Rain
11d.png	 11n.png	 Thunderstorm
13d.png	 13n.png	 snow
50d.png	 50n.png	 mist
    
http://bugs.openweathermap.org/projects/api/wiki/Weather_Data
http://bugs.openweathermap.org/projects/api/wiki/Weather_Condition_Codes
*/
?>
