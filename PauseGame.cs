using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.Match;
public class PauseGame : MonoBehaviour {

    public static bool isOn = false;

    private NetworkManager networkManager;

    void Start()
    {
       
        networkManager=NetworkManager.singleton;
    }
    public void LeaveRoom()
    {
        Debug.Log("Disconecting");
        MatchInfo matchInfo = networkManager.matchInfo;
        networkManager.matchMaker.DropConnection(matchInfo.networkId, matchInfo.nodeId, 0, networkManager.OnDropConnection);
        
        networkManager.StopHost();
        Application.LoadLevel(0);
    }


}
