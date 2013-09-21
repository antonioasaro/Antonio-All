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

    $hostname = "mysql4.000webhost.com"; 
    $username = "a7417365_all";
    $password = "a7417365_passwd";
    $dbname   = "a7417365_all"; 
    $tbname   = "weather_quotes"; 

    $dbhandle = mysql_connect($hostname, $username, $password) or die();
    $selected = mysql_select_db($dbname ,$dbhandle) or die();
    $result = mysql_query("SELECT * FROM ".$tbname." WHERE SERIAL_NUM = '".$serial_num."'");
    if (mysql_num_rows($result) > 0) { 
        print "Updated existing record.";
        mysql_query($sqlcmd = "UPDATE `weather_quotes` SET `SERIAL_NUM`='".$serial_num."',`LOCATION`='".$location."',`UNITS`='".$units."',`STOCK1`='".$stock1."',`STOCK2`='".$stock2."' WHERE 1");
    } else {
        print "Adding new record.";
        mysql_query("INSERT INTO `weather_quotes`(`SERIAL_NUM`, `LOCATION`, `UNITS`, `STOCK1`, `STOCK2`) VALUES ('".$serial_num."','".$location."','".$units."','".$stock1."','".$stock2."')");
    }
    mysql_close($dbhandle);
} else {
?>

<form method="post" action="<?php $_PHP_SELF ?>">
  <table>
    <tr>
      <td align="right">Serial Number:</td>
      <td align="left"><input type="text" name="serial_num" /></td>
      <td align="left">// First 11-digits on the watchback</td>
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
