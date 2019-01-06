#pragma strict

var coinEffect : Transform;//cand vrei sa faci ceva sa apara folosesti transform
var coinValue = 1;
function OnTriggerEnter (info : Collider) 
{
  if(info.tag == "Player")
  {
  GameMaster.currentScore += coinValue;
  var effect =Instantiate(coinEffect, transform.position, transform.rotation);
  Destroy(effect.gameObject, 3);//distruge efectul dupa 3 secunde
  Destroy(gameObject);//dispar banii

  
  }
}