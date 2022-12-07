package com.lilly.bluetoothclassic.ui

import android.bluetooth.BluetoothAdapter
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.View
import android.widget.Toast
import androidx.activity.result.ActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.databinding.DataBindingUtil
import com.lilly.bluetoothclassic.R
import com.lilly.bluetoothclassic.databinding.ActivityMainBinding
import com.lilly.bluetoothclassic.util.*
import com.lilly.bluetoothclassic.viewmodel.MainViewModel
import kotlinx.android.synthetic.main.activity_main.*
import org.koin.androidx.viewmodel.ext.android.viewModel
import java.text.SimpleDateFormat
import java.util.*

class MainActivity : AppCompatActivity() {

    private val viewModel by viewModel<MainViewModel>()

    var mBluetoothAdapter: BluetoothAdapter? = null
    var recv: String = ""
    var msg=""
    var reset="r"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val binding: ActivityMainBinding =
            DataBindingUtil.setContentView(this, R.layout.activity_main)
        binding.viewModel = viewModel

        if (!hasPermissions(this, PERMISSIONS)) {
            requestPermissions(PERMISSIONS, REQUEST_ALL_PERMISSION)
        }

        initObserving()

        survivor.setOnClickListener {
            survivor.setImageResource(R.drawable.survivor_off)
        }
        siren.setOnClickListener {
            siren.setImageResource(R.drawable.siren_off)
        }

        btn_send.setOnClickListener(send_msg)
        reset_lcd.setOnClickListener (reset_lcd_msg)
        reset_monitor.setOnClickListener (reset_monitor_msg)
    }

    val send_msg = View.OnClickListener {
        msg=txt_send.getText().toString()
        viewModel.onSendData(msg)
        txt_send.setText("")
        txt_send.requestFocus()

        if (msg==reset) reset_lcd_msg
        else {
            Util.showNotification("메시지 전송 완료 : $msg")
        }

    }

    val reset_lcd_msg  = View.OnClickListener{
        viewModel.onSendData(reset)
        Util.showNotification("LCD 모니터 초기화 : \n PUSH BUTTON!")
    }

    val reset_monitor_msg = View.OnClickListener{
        viewModel.txtRead.set("")
        Util.showNotification("출력창 초기화 완료")
        recv = ("출력 초기화 : " + getTime() + "\n")
        viewModel.txtRead.set(recv)
    }


    private val startForResult = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result: ActivityResult ->
        if (result.resultCode == RESULT_OK) {
            val intent = result.data
            viewModel.onClickConnect()
        }
    }

    private fun getTime(): String {
        val now=System.currentTimeMillis()
        val date = Date(now)
        val dateFormat = SimpleDateFormat("yyyy-MM-dd hh:mm:ss")
        val setTime = dateFormat.format(date)

        return setTime
    }

    private fun initObserving(){

        //Progress
        viewModel.inProgress.observe(this) {
            if (it.getContentIfNotHandled() == true) {
                viewModel.inProgressView.set(true)
            } else {
                viewModel.inProgressView.set(false)
            }
        }
        //Progress text
        viewModel.progressState.observe(this) {
            viewModel.txtProgress.set(it)
        }

        //Bluetooth On 요청
        viewModel.requestBleOn.observe(this) {
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startForResult.launch(enableBtIntent)
        }

        //Bluetooth Connect/Disconnect Event
        viewModel.connected.observe(this) {
            if (it != null) {
                if (it) {
                    viewModel.setInProgress(false)
                    viewModel.btnConnected.set(true)
                    Util.showNotification("디바이스와 연결되었습니다.")
                    recv=("디바이스 연결 : " + getTime() + "\n")
                    viewModel.txtRead.set(recv)
                } else {
                    viewModel.setInProgress(false)
                    viewModel.btnConnected.set(false)
                    Util.showNotification("디바이스와 연결이 해제되었습니다.")
                    onResume()
                }
            }
        }

        //Bluetooth Connect Error
        viewModel.connectError.observe(this) {
            Util.showNotification("디바이스 연결에 실패하였습니다. 기기를 확인해주세요.")
            viewModel.setInProgress(false)
        }

        //Data Receive
        viewModel.putTxt.observe(this) {
            if (it != null) {
                if (it.length > 12) {
                    Util.showNotification("12글자를 초과하여 문자를 보낼 수 없습니다.")
                } else {
                    recv += (getTime() + " : " + it + "\n")
                    sv_read_data.fullScroll(View.FOCUS_DOWN)
                    viewModel.txtRead.set(recv)

                    if (it.equals("구조요청")) {
                        siren.setImageResource(R.drawable.siren_on)
                        Util.showNotification("구조 요청이 왔습니다!")
                    }

                    if (it.equals("생존자발견")) {
                        survivor.setImageResource(R.drawable.survivor_on)
                        Util.showNotification("생존자를 발견하였습니다!")
                    }
                }

            }
        }
    }
    private fun hasPermissions(context: Context?, permissions: Array<String>): Boolean {
        for (permission in permissions) {
            if (context?.let { ActivityCompat.checkSelfPermission(it, permission) }
                    != PackageManager.PERMISSION_GRANTED
            ) {
                return false
            }
        }
        return true
    }
    // Permission check
    override fun onRequestPermissionsResult(
            requestCode: Int,
            permissions: Array<String?>,
            grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            REQUEST_ALL_PERMISSION -> {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(this, "블루투스 연결에 성공하였습니다!", Toast.LENGTH_SHORT).show()
                } else {
                    requestPermissions(permissions, REQUEST_ALL_PERMISSION)
                    Toast.makeText(this, "어플을 이용하기 위하여 블루투스 권한이 필요합니다.", Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.txtRead.set("디바이스가 보낸 메시지가 아직 없습니다.")
    }

    override fun onPause(){
        super.onPause()
        viewModel.unregisterReceiver()
    }

    override fun onBackPressed() {
        //super.onBackPressed()
        viewModel.setInProgress(false)
    }

}