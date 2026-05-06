package com.example.pruebasnfc_apdu

import android.content.Intent
import android.os.Bundle
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import android.content.pm.PackageManager
import android.nfc.NfcAdapter
import android.nfc.cardemulation.HostApduService
import android.os.Vibrator
import android.os.VibratorManager
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast

const val TAG = "APDU"


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

        //TODO revisar que el comando sea para nuestra app, por el momento simplemente responder
        val vibrator = getSystemService(VIBRATOR_SERVICE) as Vibrator
        vibrator.vibrate(100)


        // Return a "Success" response (0x9000 in APDU language) plus DEADCAFE bytes
        val response = byteArrayOf(

            0xDE.toByte(),
            0xAD.toByte(),
            0xCA.toByte(),
            0xFE.toByte(),
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

        //pruebas de boton
        val buttonAction = findViewById<Button>(R.id.buttonAction)
        buttonAction.setOnClickListener {
            Toast.makeText(this, "Boton pulsado", Toast.LENGTH_SHORT).show()
            Log.d(TAG, "Boton pulsado")
        }





        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
    }
}