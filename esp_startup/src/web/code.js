



async function getAccessPoints() {
    var wrapper = document.getElementById("apps_wrapper");
    var options = document.getElementById("ssid");
    const response = await fetch("/scan");
    const aps = await response.json();
    console.log(aps);
    wrapper.innerHTML = '<div class="ap_row">Number of scanned APs:' + aps.length + '</div>'
    aps.forEach(element => {
        let ap_row = '<div class="ap_row"><a href="#' + element.ssid + '">' + element.ssid + '</a></div>';
        let select = '<option value="' + element.ssid + '">' + element.ssid + '</option>'
        wrapper.innerHTML = wrapper.innerHTML.concat(ap_row);
        options.innerHTML = options.innerHTML.concat(select);
    });
}

/*
    static wifi_c_status_t wifi_c_status = {
    .wifi_initialized = false,
    .netif_initialized = false,
    .wifi_mode = WIFI_C_NO_MODE,
    .even_loop_started = false,
    .sta_started = false,
    .ap_started = false,
    .scan_done = false,
    .sta_connected = false,
    .ip = "0.0.0.0",
    .ap_ssid = "none",
};
*/

async function getDeviceInfo() {
    response = await fetch("/state");
    const state = await response.json();
    console.log(state);
    
    response = await fetch("/saved_ap");
    const saved_ap = await response.json();
    console.log(saved_ap);


    getApInfo(state);
    getStaInfo(state, saved_ap);
}

async function getStoredAP() {
    const response = await fetch("/saved_ap");
    const saved_ap = await response.json();
    console.log(saved_ap);

    return saved_ap.stored_ssid;
}

async function getApInfo(state) {
    var wrapper = document.getElementById("ap_info");
    const newline = "</br>";

    wrapper.innerHTML = "AP SSID:" + state.ap_ssid + newline + "Current IP: " + state.ap_ip + newline + "Wifi mode: " + state.wifi_mode;
}

async function getStaInfo(state, saved_ap) {
    var wrapper = document.getElementById("sta_info");
    const newline = "</br>";

    wrapper.innerHTML = "Current IP: " + state.sta_ip + newline + "Wifi mode: " + state.wifi_mode + newline + "Connected to: " + state.sta_ssid + newline + "Saved APs: " + saved_ap.stored_ssid;
}

async function postConnectRequest() {
    var ssid_field = document.getElementById("ssid");
    var ssid = ssid_field.options[ssid_field.selectedIndex].value;
    var password = document.getElementById("password").value;
    let headers = new Headers();
    headers.append("Content-type", "text/plain");
    headers.append("X-Custom-ssid", ssid);
    headers.append("X-Custom-password", password);
    console.log(ssid);
    console.log(password)
    console.log(headers);

    let response = await fetch("/connect", {
        method: "POST",
        body: "connect",
        headers: headers
    });
    let text = await response.text();
    alert(text.slice(0, 80));
}

window.onload = (event) => {
    getDeviceInfo();
    getAccessPoints();
};