using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class PlayerScoarboardItem : MonoBehaviour {
    [SerializeField]
    Text usernameText;
    [SerializeField]
    Text killsText;
    [SerializeField]
    Text deathsText;
    
	public void Setup (int kills, int deaths)
    {
        
        Player _player = GameManager.GetPlayer(name);
        usernameText.text = _player.name;
        killsText.text = "Kills:  " + kills;
        deathsText.text = "Deaths: " + deaths;
        
    }
}
