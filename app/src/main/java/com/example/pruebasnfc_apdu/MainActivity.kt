package com.example.pruebasnfc_apdu

import android.os.Bundle
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import android.content.pm.PackageManager
import android.nfc.NfcAdapter
import android.nfc.cardemulation.HostApduService
import android.os.Vibrator
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast

const val TAG = "APDU"

//Funciones de ayuda para pasar de bytes a Int
fun byteArrayToInt(bytes: ByteArray): Int
{
    return ((bytes[0].toInt() and 0xFF) shl 24) or
            ((bytes[1].toInt() and 0xFF) shl 16) or
            ((bytes[2].toInt() and 0xFF) shl 8)  or
            (bytes[3].toInt() and 0xFF)
}
fun Int2ByteArray(value: Int): ByteArray {
    return byteArrayOf(
        (value shr 24).toByte(),
        (value shr 16).toByte(),
        (value shr 8).toByte(),
        value.toByte()
    )
}


//el servicio corre en el background de Android
//debe registrarse en el androidmanifest
class MyHostAPDUService : HostApduService() {
    override fun onDeactivated(reason: Int) {
        Log.d(TAG, "onDeactivated reason="+ reason.toString())
    }

    //llamado cuando el dispositivo detecta un nuevo dispositivo NFC
    override fun processCommandApdu(
        commandApdu: ByteArray?,
        extras: Bundle?
    ): ByteArray? {

        if (commandApdu == null) {
            Log.d(TAG, "processCommandApdu null, returning 6F 00")
            //el estándar pide que se anunce error "no precise diagnosis"
            return byteArrayOf(0x6F.toByte(), 0x00.toByte())
        }
        Log.d("APDU", "APDU IN: ${commandApdu.joinToString(" ") { "%02X".format(it) }}")
        val vibrator = getSystemService(VIBRATOR_SERVICE) as Vibrator
        vibrator.vibrate(100)
        //TODO revisar que el comando sea para nuestra appId, por el momento simplemente responder

        val prefs = getSharedPreferences("CREDENCIAL_ID", MODE_PRIVATE)
        val id = prefs.getInt("ID", 0)
        Log.d(TAG, "ID: " + id.toString())
        val idbytes = Int2ByteArray(id)

        // Return a "Success" response (0x9000 in APDU language) plus DEADCAFE bytes
        val response = byteArrayOf(
            idbytes[0],
            idbytes[1],
            idbytes[2],
            idbytes[3],
            0x90.toByte(),
            0x00.toByte())

        return response
    }

}

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)

        val labelStatus = findViewById<TextView>(R.id.label_info)
        val hasHCE = packageManager.hasSystemFeature(PackageManager.FEATURE_NFC_HOST_CARD_EMULATION)
        val nfcAdapter = NfcAdapter.getDefaultAdapter(this)
        val hasNFC = nfcAdapter != null

        if(!hasNFC) {
            labelStatus.text = getString(R.string.no_nfc)
        }
        else
        {
            if(!hasHCE) {
                labelStatus.text = getString(R.string.no_hce)
            }
            //todo bien, continuar con la app
            else{
                labelStatus.text = "todo bien "
                //val intent = Intent(this, MyHostAPDUService::class.java)
                //Toast.makeText(this, "apduservice intent created", Toast.LENGTH_SHORT).show()


            }

        }

        //boton con la credencial 1
        val buttonAction = findViewById<Button>(R.id.buttonCred1)
        buttonAction.setOnClickListener {
            Toast.makeText(this, "Credencial cambiada a 1", Toast.LENGTH_SHORT).show()
            Log.d(TAG, "new ID set")
            //Usando SharedPrefs cambiamos el valor del ID enviado
            val prefs = getSharedPreferences("CREDENCIAL_ID", MODE_PRIVATE)
            val idbytes =  byteArrayOf(
                0xDE.toByte(),
                0xAD.toByte(),
                0xCA.toByte(),
                0xFE.toByte()
            )
            val id = byteArrayToInt(idbytes) as Int
            Log.d(TAG, "new ID: " + id.toString())
            prefs.edit().putInt( "ID", byteArrayToInt(idbytes)).apply()
            labelStatus.text="Credencial A"
        }

        //boton con la credencial 2
        val buttonCredB = findViewById<Button>(R.id.buttonCred2)
        buttonCredB.setOnClickListener {
            Toast.makeText(this, "Credencial cambiada a 2", Toast.LENGTH_SHORT).show()
            Log.d(TAG, "new ID set")
            //Usando SharedPrefs cambiamos el valor del ID enviado
            val prefs = getSharedPreferences("CREDENCIAL_ID", MODE_PRIVATE)
            //deadc0de no esta en la lista de usuarios del firmware
            val idbytes = byteArrayOf(
                0xDE.toByte(),
                0xAD.toByte(),
                0xC0.toByte(),
                0xDE.toByte()
            )
            val id = byteArrayToInt(idbytes) as Int
            Log.d(TAG, "new ID: " + id.toString())
            prefs.edit().putInt("ID", byteArrayToInt(idbytes)).apply()
            labelStatus.text = "Credencial B"
        }

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
    }
}