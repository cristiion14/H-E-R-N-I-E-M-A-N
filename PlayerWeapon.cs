using UnityEngine;

[System.Serializable]
public class PlayerWeapon {

    public string name = "Glock";
    public int damage = 10;
    public float range = 500f;
    public float fireRate = 0f;
    public GameObject graphics;
    public int maxBullets = 20;
    [HideInInspector]
    public int bullets;

    public float reloadTime = 1f;  

    public PlayerWeapon()
    {
        bullets = maxBullets;
    }
}
