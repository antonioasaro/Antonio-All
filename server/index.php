<html>
<head>
<title>Update MySQL Database</title>
</head>
<body>

<?PHP
if (isset($_POST['add'])) {
    $serial_num = $_POST['serial_num'];
    $location = $_POST['location'];
    $units = $_POST['units'];
    $stock1 = $_POST['stock1'];
    $stock2 = $_POST['stock2'];

    $hostname = "localhost"; 
    $username = "root";
    $password = "fawnridge";
    // $hostname = "mysql4.000webhost.com"; 
    // $username = "a7417365_all";
    // $password = "a7417365_passwd";
    $dbname   = "a7417365_all"; 
    $tbname   = "weather_quotes"; 

    $dbhandle = mysql_connect($hostname, $username, $password) or die();
    $selected = mysql_select_db($dbname ,$dbhandle) or die();
    $result = mysql_query("INSERT INTO ".$tbname." (`SERIAL_NUM`, `LOCATION`, `UNITS`, `STOCK1`, `STOCK2`) VALUES (".$serial_num.",".$location.",".$units.",".$stock1.",".$stock2.")");
    if ($result) {
       print "<b>Serial Number:</b> $serial_num,  <b>Location:</b> $location, <b>Units:</b> $units, <b>Stock1:</b> $stock1, <b>Stock2:</b> $stock2";
       print "<br>Successfully entered data.";
    } else { 
	    die('Could not enter data: ' . mysql_error()); 
    }
    mysql_close($dbhandle);
} else {
?>

<form method="post" action="<?php $_PHP_SELF ?>">
  <table>
    <tr>
      <td align="right">Serial Number:</td>
      <td align="left"><input type="text" name="serial_num" /></td>
      <td align="left">// 12-digits on the watchback</td>
    </tr>
    <tr>
      <td align="right">Location:</td>
      <td align="left"><input type="text" name="location" /></td>
      <td align="left">// e.g. Toronto,Canada</td>
    </tr>
    <tr>
      <td align="right">Units:</td>
      <td align="left"><input type="text" name="units" /></td>
      <td align="left">// Metric or Imperial</td>
    </tr>
    <tr>
      <td align="right">Stock1:</td>
      <td align="left"><input type="text" name="stock1" /></td>
      <td align="left">// e.g. IBM</td>
    </tr>
    <tr>
      <td align="right">Stock2:</td>
      <td align="left"><input type="text" name="stock2" /></td>
      <td align="left">// e.g. GOOG</td>
    </tr>
  </table>
  <br> <input name="add" type="submit" id="add">
</form>
<?php
}
?>

</body>
</html>
