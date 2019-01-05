using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Scoreboard : MonoBehaviour {
    [SerializeField]
    GameObject playerScoarboardItem;
    [SerializeField]
    Transform playerScoarboardList;
    public Text killText;
    public Text deathText;
    public Text playerName;
    void OnEnable()
    {
        //Get an array of players
        Player[] players = GameManager.GetAllPlayers();
        foreach(Player player in players)
        {
            killText.text = "Kills: " + player.kills;
            deathText.text = "Deaths: " + player.deaths;
            playerName.text = player.name;
        }
        //loop through and set up a list item for each one
        //setting the ui elements equal relevant data
    }
    void OnDisable()
    {
        //clean up our list of items
    }
}
