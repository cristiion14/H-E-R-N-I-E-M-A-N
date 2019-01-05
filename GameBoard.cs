using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameBoard : MonoBehaviour {

    private static int boardWidth = 28;
    private static int boardHeight = 36;

    public AudioClip backroundAudioNoramal;
    public AudioClip backroundAudioFrightened;

    public int totalPellets = 0;
    public int score = 0;

    public GameObject[,] board = new GameObject[boardWidth, boardHeight];
	// Use this for initialization
	void Start () {
        //storing all the objects from the scene in objects array
        Object[] objects = GameObject.FindObjectsOfType(typeof(GameObject));
        foreach (GameObject obj in objects)
        {
            Vector2 pos = obj.transform.position;
            if(obj.name != "PacMan" && obj.name!= "Nodes" && obj.name!= "NotNodes" && obj.name!= "Maze" && obj.name!= "Pellets" && obj.tag != "Maze"&&obj.tag!="Ghost"&&obj.tag!="ghostHome")
            {
                if (obj.GetComponent<Tile>() != null)
                {
                      if(obj.GetComponent<Tile>().isPallet || obj.GetComponent<Tile>().isSuperPallet)      //if found pallet or super pallet
                    {
                        totalPellets++;                 
                    }
                }    
                board[(int)pos.x, (int)pos.y] = obj;
            }
            else
            {
                Debug.Log("Found PacMan at: " + pos);
            }
        }
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
