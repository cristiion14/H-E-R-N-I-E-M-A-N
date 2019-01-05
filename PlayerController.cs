using UnityEngine;
using UnityEngine.SceneManagement;
[RequireComponent(typeof(PlayerMotor))]
public class PlayerController : MonoBehaviour {

    [SerializeField]
    private float speed = 5f;
    [SerializeField]
    private float lookSensitivity = 3f;
    private PlayerMotor motor;
    private bool isFalling = false;
    private Rigidbody Rb;
    [SerializeField]
    private float jumpForce=10f;
    private float seconds;
    private int count;
    private bool isTiming = false;

    
    void Start()
    {
        motor = GetComponent<PlayerMotor>();
        Rb = GetComponent<Rigidbody>();
        seconds = 5.0f;
        count = 0;
    }
    void FixedUpdate()
    {
        if (PauseGame.isOn)
        {
            if (Cursor.lockState != CursorLockMode.None)
                Cursor.lockState = CursorLockMode.None;
            motor.Move(Vector3.zero);
                motor.Rotate(Vector3.zero);
                motor.RotateCamera(0f);
            
            return;
        }
        if(Cursor.lockState != CursorLockMode.Locked)
        {
            Cursor.lockState = CursorLockMode.Locked;
        }
        //calculate movement velocity as a 3d vector
        float _xMov = Input.GetAxisRaw("Horizontal");
        float _zMov = Input.GetAxisRaw("Vertical");


        Vector3 _movHorizontal = transform.right * _xMov;
        Vector3 _movVertical = transform.forward * _zMov;
        Vector3 _velocity = (_movHorizontal + _movVertical).normalized * speed;
        //apply movement
        motor.Move(_velocity);
        //calculate player rotation as a 3d vector (turning around)
        float _yRot = Input.GetAxisRaw("Mouse X");
        
        Vector3 _rotation = new Vector3(0f, _yRot, 0f) * lookSensitivity * Time.fixedDeltaTime;
        //apply rotation
        motor.Rotate(_rotation);
        if (Input.GetButtonDown("Jump") && isFalling == false)
        {
            Rb.velocity = new Vector3(0, jumpForce, 0);
            isFalling = true;
        }
      
        /*controller movement
        float _cRot = Input.GetAxisRaw("rightAnalog");
        Vector3 _Rotation = new Vector3(0f, _cRot, 0f) * lookSensitivity;
        motor.Rotate(_Rotation);
         */

        
        //calculate camera rotation as a 3d vector (turning around)
        float _xRot = Input.GetAxisRaw("Mouse Y");
        float  _cameraRotation = _xRot * lookSensitivity * Time.fixedDeltaTime;
        //apply camera rotation
        motor.RotateCamera(_cameraRotation);
         if (isTiming == true)
        {
            seconds -= Time.deltaTime;
        }
        if (seconds <= 0.0f)
        {
            isTiming = false;
            count = count - 1;
            speed = speed - 10.0f;
            seconds = seconds + 7;
        }
    }
       
    

    void OnCollisionStay()
    {
        isFalling = false;
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.tag == "speedboost")
        {
            other.gameObject.SetActive(false);
            count = count + 1;
            speed = speed + 10.0f;
            seconds = 5;
            isTiming = true;
        }

    }
}
