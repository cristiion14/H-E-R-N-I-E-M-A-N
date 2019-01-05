using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class PlayerUI : MonoBehaviour {
    [SerializeField]
    GameObject pauseMenu;
    [SerializeField]
    Text ammoText;
    [SerializeField]
    GameObject scoreboard;
 
    void Start ()
    {
        PauseGame.isOn = false ;
    }

    void Update ()
    {
        
        if(Input.GetKeyDown(KeyCode.Escape))
        {  
            TogglePauseMenu();
        }
        if(Input.GetKeyDown(KeyCode.Tab))
        {
            scoreboard.SetActive(true);
        }
        else if (Input.GetKeyUp(KeyCode.Tab))
        {
            scoreboard.SetActive(false); 
        }
    }
    void TogglePauseMenu()
    {
        pauseMenu.SetActive(!pauseMenu.activeSelf);
        PauseGame.isOn = pauseMenu.activeSelf;
    }
   
}
