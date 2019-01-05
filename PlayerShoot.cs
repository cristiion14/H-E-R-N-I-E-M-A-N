using UnityEngine;
using UnityEngine.Networking;


[RequireComponent (typeof (WeaponManager))]
public class PlayerShoot : NetworkBehaviour
{

    private const string PLAYER_TAG = "Player";



    [SerializeField]
    AudioSource shootSound;
  
    [SerializeField]
    private Camera cam;

    [SerializeField]
    private LayerMask mask;
    private PlayerWeapon currentWeapon;
    private WeaponManager weaponManager;
    void Start()
    {
        
    
        if (cam == null)
        {
            Debug.LogError("PlayerShoot: No camera referenced!");
            this.enabled = false;
        }
        weaponManager = GetComponent<WeaponManager>();
    }

    void Update()
    {
    
        currentWeapon = weaponManager.GetCurrentWeapon();

        if (PauseGame.isOn)
            return; 

        if (currentWeapon.fireRate <= 0)
        {
            if (Input.GetButtonDown("Fire1"))
            {
                Shoot();
                
            }
        }
        else
        {
            if (Input.GetButtonDown("Fire1") && isLocalPlayer)
            {
               
                InvokeRepeating("Shoot", 0f, 1f / currentWeapon.fireRate);
                InvokeRepeating("PlaySound", 0f, 1 / currentWeapon.fireRate);
               
            }

            else if (Input.GetButtonUp("Fire1"))
            {
                CancelInvoke("Shoot");
                CancelInvoke("PlaySound");
            }
        }

    }
       
    
        void PlaySound()
    {
        shootSound.Play();
    }
    
    //Is called on the server when a player shoots
    [Command]

    void CmdOnShoot()
    {
        RpcDoShootEffect();
    }
    //is called on all clients when we need to do
    // a shoot effect
    [ClientRpc]
    void RpcDoShootEffect()
    {
     
        weaponManager.GetCurrentGraphics().muzzleFlash.Play();
    }
    // Is called on the server when we hit something
    // Takes in the hit point and the normal of the surface
    [Command]
    void CmdOnHit (Vector3 _pos, Vector3 _normal)
    {
        RpcDoHitEffect(_pos, _normal);
    }
    // Is called on all clients
    // Here we can spawn in cool effects
    [ClientRpc]
    void RpcDoHitEffect(Vector3 _pos, Vector3 _normal)
    {
        GameObject _hitEffect =  (GameObject)Instantiate(weaponManager.GetCurrentGraphics().hitEffectPrefab, _pos, Quaternion.LookRotation(_normal));
        Destroy(_hitEffect, 2f);    
    }

    [Client]
    void Shoot()
    {
        
        if(!isLocalPlayer)
        {
            return;
        }
    
        if(currentWeapon.bullets<=0)
        {
            weaponManager.Reload();
            return;
        }
            
        currentWeapon.bullets--;
        Debug.Log("Remaining bullets: " + currentWeapon.bullets);
        //We are shooting, Call the OnShoot method on server.
        CmdOnShoot();
        RaycastHit _hit;
        if (Physics.Raycast(cam.transform.position, cam.transform.forward, out _hit, currentWeapon.range, mask))
        {
            if(_hit.collider.tag==PLAYER_TAG)
            {
                CmdPlayerShoot(_hit.collider.name, currentWeapon.damage, transform.name);
            }
            // We hit something, call the OnHit method on the server
            CmdOnHit(_hit.point, _hit.normal);
        }
    }
    [Command]//means that this command in on the server

    void CmdPlayerShoot(string _playerID, int _damage, string _sourceID)
    {
        Debug.Log(_playerID + " has been shot");

        Player _player = GameManager.GetPlayer(_playerID);
        _player.RpcTakeDamage(_damage, _sourceID);

    }

}
