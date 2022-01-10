<?php

$save=0;
if(isset($_POST["check"])){
	$save=1;
}
//echo phpinfo();

if(isset($_POST["check"])){
$inf=1;
}
$inf=isset($_POST["xml"]) ? $_POST["xml"] : "testXMLPacket1.xml" ;
//$ouf="test.xml";
//$xml = simplexml_load_file("test.xml");
//$movies=new SimpleXMLElement($xmlstr);
//echo $xml->asXML();

//echo getcwd();
$xmlPacket=simplexml_load_file($inf);
if($save==1){
	$xmlPacket->saveEnv["save"]="true";
}
$xml=$xmlPacket->asXML();

//$url="http://localhost/cgi-bin/ddgem?echo=fromPHP&j=345&aar=hy";

$url=isset($_POST["aws"]) ? "http://dd.virtualpsyclab.net/ddgem/ddgem" : "http://localhost/cgi-bin/ddgem";

//echo $xmlPacket->asXML($ouf);
//header("Location: ".$url);

//https://thisinterestsme.com/php-send-xml-post/
//Initiate cURL
$curl = curl_init($url);
 
//Set the Content-Type to text/xml.
curl_setopt ($curl, CURLOPT_HTTPHEADER, array("Content-Type: text/xml"));
 
//Set CURLOPT_POST to true to send a POST request.
curl_setopt($curl, CURLOPT_POST, true);
 
//Attach the XML string to the body of our request.
curl_setopt($curl, CURLOPT_POSTFIELDS, $xml);
 
//Tell cURL that we want the response to be returned as
//a string instead of being dumped to the output.
curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
 
//Execute the POST request and send our XML.
$answer=curl_exec($curl);
 
//Do some basic error checking.
if(curl_errno($curl)){
    throw new Exception(curl_error($curl));
}
 
//Close the cURL handle.
curl_close($curl);

$start=strpos($answer,'<body>')+6;
$stop=strpos($answer,'</body>');
echo $start;
echo $stop;
$data=substr($answer,$start,$stop-$start);
$values=explode("\t",$data);
var_dump($answer);
var_dump($data);
var_dump($values);
echo($values[0]);
echo($values[1]);
echo($values[2]);
//Print out the response output.
//echo $result;
exit;
?>
