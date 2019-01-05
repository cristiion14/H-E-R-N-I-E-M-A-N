using UnityEngine;
using System.Collections;

public class CoolDown : MonoBehaviour {


    public float speedcoolDown = 5.0f;
    public float healthcoolDown = 5.0f;
    public float shieldcoolDown = 5.0f;
    public GameObject SpeedBoost;
    public GameObject Health;
    public GameObject Shield;

	// Use this for initialization
	void Start () {
        SpeedBoost = GameObject.Find("SpeedBoost");
        Health = GameObject.Find("Health");
        Shield = GameObject.Find("Shield");
        speedcoolDown = 5;
        healthcoolDown = 5;
        shieldcoolDown = 5;

    }
	
	// Update is called once per frame
	void Update () {
	
        if (SpeedBoost.activeInHierarchy == false)
        {
            speedcoolDown -= Time.deltaTime;
        }

        if (Health.activeInHierarchy == false)
        {
            healthcoolDown -= Time.deltaTime;
        }

        if (Shield.activeInHierarchy == false)
        {
            shieldcoolDown -= Time.deltaTime;
        }

        if (speedcoolDown <= 0.0f)
        {
            SpeedBoost.SetActive(true);
            speedcoolDown = speedcoolDown + 5;
        }

        if (healthcoolDown <= 0.0f)
        {
            Health.SetActive(true);
            healthcoolDown = healthcoolDown + 5;
        }

        if (shieldcoolDown <= 0.0f)
        {
            Shield.SetActive(true);
            shieldcoolDown = shieldcoolDown + 5;
        }
    }
}
