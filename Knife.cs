using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement; 
public class Knife : MonoBehaviour {

    public float force = 5f;//the force of the swipe
    public float torque = 20f;//the force of the rotation
    public Text scoreText;
    public Rigidbody rb;
    private Vector2 startSwipe;//the start position
    private Vector2 endSwipe; //the end position
    private float startTime;//time when the knife starts to fly
    private static int score = 0;
	// Use this for initialization
	void Start () {
        
	}
	
	// Update is called once per frame
	void Update () {
        scoreText.text = "Score: " + score;
		if(Input.GetMouseButtonDown(0))
        {
            startSwipe = Camera.main.ScreenToViewportPoint(Input.mousePosition);//gives the position of the mouse

        }
        if(Input.GetMouseButtonUp(0))
        {
            endSwipe = Camera.main.ScreenToViewportPoint(Input.mousePosition);
            Swipe();
        }
	}
    void Swipe()
    {
        rb.isKinematic = false;
        startTime = Time.time;
        Vector2 swipe = endSwipe - startSwipe;
        Debug.Log(swipe);
        rb.AddForce(swipe * force, ForceMode.Impulse);
        rb.AddTorque(0f, 0f, -torque, ForceMode.Impulse);
    }
    //cand a atins un collider
    void OnTriggerEnter(Collider col)
    {
        if(col.tag == "Block")                                                             
        {
            rb.isKinematic = true;
            score++;
        }
        else
        {
            Restart();
        }
       
    }
    // in cazul in care nu nimereste coliderul
    void OnCollisionEnter()
    {
        float timeInAir = Time.time - startTime;
        if(!rb.isKinematic && timeInAir >= .05f)
        {
            Restart();
        }
        Debug.Log("FAIL...");
        
        
    }
    void Restart()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
    }

}
