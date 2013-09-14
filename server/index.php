<?PHP

if (!isset($_GET['stock1']) || !isset($_GET['stock2'])) die();

$stock1 = $_GET['stock1'];
$stock2 = $_GET['stock2'];

$json1  = curl_get('http://dev.markitondemand.com/Api/Quote/json?symbol='.$stock1);
$json2  = curl_get('http://dev.markitondemand.com/Api/Quote/json?symbol='.$stock2);

$quote1 = process_quotes($json1, 1+0*3);
$quote2 = process_quotes($json2, 1+1*3);

print json_encode($quote1 + $quote2);

// Obtain Quote Info
// $quote = file_get_contents('http://dev.markitondemand.com/Api/Quote/json?symbol='.$stock);
// or with curl
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

function process_quotes($quote, $i) {
    $json_output = json_decode(utf8_decode($quote));
    if (!$json_output) die(); 

    $symb = $json_output->Data->Symbol;
    $last = $json_output->Data->LastPrice;
    $perc = $json_output->Data->ChangePercent;
	
    $result = array();
    $result[$i+0] = $symb;
    $result[$i+1] = array('I', round($last, 2) * 100);
    $result[$i+2] = array('I', round($perc, 1) * 10);
    return $result;
}

/*
stdClass Object
(
    [Data] => stdClass Object
        (
            [Status] => SUCCESS
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

)*/
?>
