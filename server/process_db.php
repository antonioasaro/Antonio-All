<?PHP
if (isset($_POST['submit'])) {
    $serial_num = $_POST['serial_num'];
    $location = str_replace(' ', '', $_POST['location']);
    $units = $_POST['units'];
    $stock1 = $_POST['stock1'];
    $stock2 = $_POST['stock2'];
    if (strlen($serial_num) != 12) die("Error: Serial Number MUST be 12-digits long.");
    $serial_num = substr($serial_num, 0, -1);

    $hostname = "mysql4.000webhost.com"; 
    $username = "a7417365_all";
    $password = "a7417365_passwd";
    $dbname   = "a7417365_all"; 
    $tbname   = "weather_quotes"; 

    $dbhandle = mysql_connect($hostname, $username, $password) or die();
    $selected = mysql_select_db($dbname ,$dbhandle) or die();
    $result = mysql_query("SELECT * FROM ".$tbname." WHERE SERIAL_NUM = '".$serial_num."'");
    if (mysql_num_rows($result) > 0) { 
        print "Updating existing record ...";
        $result = mysql_query("UPDATE `weather_quotes` SET `LOCATION`='".$location."',`UNITS`='".$units."',`STOCK1`='".$stock1."',`STOCK2`='".$stock2."' WHERE `SERIAL_NUM`='".$serial_num."'");
	if ($result) print "<br>Success.";
    } else {
        print "Adding new record ...";
        $result = mysql_query("INSERT INTO `weather_quotes`(`SERIAL_NUM`, `LOCATION`, `UNITS`, `STOCK1`, `STOCK2`) VALUES ('".$serial_num."','".$location."','".$units."','".$stock1."','".$stock2."')");
	if ($result) print "<br>Success.";
    }
    mysql_close($dbhandle);
}
?>
