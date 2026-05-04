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
import android.util.Log
import android.widget.TextView
import android.widget.Toast

const val TAG = "APDU"


//el servicio corre en el background de Android
//debe registrarse en el androidmanifest
class MyHostAPDUService : HostApduService() {
    override fun onDeactivated(reason: Int) {
        //TODO("Not yet implemented")
    }

    //llamado cuando el dispositivo detecta un nuevo dispositivo NFC
    override fun processCommandApdu(
        commandApdu: ByteArray?,
        extras: Bundle?
    ): ByteArray? {
        Log.d(TAG, "processCommandApdu")

        // Return a "Success" response (0x9000 in APDU language)
        return byteArrayOf(0x90.toByte(), 0x00.toByte())

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







        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
    }
}