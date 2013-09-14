<?PHP

if (!isset($_GET['location']) || !isset($_GET['units'])) die();

$location = $_GET['location'];
$units = $_GET['units'];

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

//    $symb = $json_output->Data->Symbol;
//    $last = $json_output->Data->LastPrice;
//    $perc = $json_output->Data->ChangePercent;
    print json_encode($json_output);
//    $abc = $json_output->{'country'};
//    print $abc;
	
    $result = array();
    $result[1] = "cloudy";
    $result[2] = array('I', round(6, 1) * 100);
    return $result;
}

/*
stdClass Object
(
    [Data] => stdClass Object
        (
            [Country] => CA
            [Name] => Facebook Inc
            [Symbol] => FB
            [LastPrice] => 31.91
            [Change] => -1.12
            [ChangePercent] => -3.3908567968514
            [Timestamp] => Fri May 25 16:00:05 UTC-04:00 2012
            [MarketCap] => 20214729720
            [Volume] => 37189630
            [ChangeYTD] => 0
            [ChangePercentYTD] => 0
            [High] => 32.95
            [Low] => 31.11
            [Open] => 32.9
        )
*/
?>
