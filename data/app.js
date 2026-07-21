//==========================================================
// World Clock
// app.js
//==========================================================

"use strict";

//==========================================================

let clocks = [];
let timezones = [];

//==========================================================
// Tabs
//==========================================================

async function showPage(pageName, button)
{
    document
        .querySelectorAll(".page")
        .forEach(p => p.classList.remove("active"));

    document
        .querySelectorAll(".tabButton")
        .forEach(b => b.classList.remove("active"));

    document
        .getElementById(pageName)
        .classList.add("active");

    button.classList.add("active");

    switch(pageName)
    {
        case "network":
            loadNetwork();
            break;

        case "clocks":
            if (timezones.length === 0)
                await loadTimezones();
            loadClocks();
            break;

        case "system":
            loadSystem();
            break;
    }
}

//==========================================================
// Network
//==========================================================

function toggleDHCPFields()
{
    const dhcp = document.getElementById("wifi_dhcp").checked;
    document.getElementById("staticIPFields").style.display =
        dhcp ? "none" : "block";
}

//==========================================================

async function loadNetwork()
{
    try
    {
        const response =
            await fetch("/api/network");

        const cfg =
            await response.json();

        document.getElementById("wifi_ssid").value =
            cfg.ssid || "";

        document.getElementById("wifi_password").value =
            cfg.password || "";

        const dhcp = (cfg.dhcp !== undefined) ? cfg.dhcp : true;
        document.getElementById("wifi_dhcp").checked = dhcp;

        document.getElementById("wifi_ip").value =
            cfg.ip || "192.168.1.100";

        document.getElementById("wifi_subnet").value =
            cfg.subnet || "255.255.255.0";

        document.getElementById("wifi_gateway").value =
            cfg.gateway || "192.168.1.1";

        document.getElementById("wifi_dns").value =
            cfg.dns || "8.8.8.8";

        toggleDHCPFields();
    }
    catch(e)
    {
        alert("Network read error");
    }
}

//==========================================================

async function saveNetwork()
{
    const dhcp = document.getElementById("wifi_dhcp").checked;

    const cfg =
    {
        ssid:     document.getElementById("wifi_ssid").value,
        password: document.getElementById("wifi_password").value,
        dhcp:     dhcp,
        ip:       document.getElementById("wifi_ip").value,
        subnet:   document.getElementById("wifi_subnet").value,
        gateway:  document.getElementById("wifi_gateway").value,
        dns:      document.getElementById("wifi_dns").value
    };

    try
    {
        const response =
            await fetch(
                "/api/network",
                {
                    method:"POST",

                    headers:
                    {
                        "Content-Type":"application/json"
                    },

                    body:JSON.stringify(cfg)
                });

        if(response.ok)
            alert("Network saved.");
        else
            alert("Save error.");
    }
    catch(e)
    {
        alert("Communication error.");
    }
}

//==========================================================
// System
//==========================================================

async function loadSystem()
{
    try
    {
        const response =
            await fetch("/api/system");

        const sys =
            await response.json();

        document.getElementById("chipModel").textContent =
            sys.chipModel;

        document.getElementById("chipRevision").textContent =
            sys.chipRevision;

        document.getElementById("cpuMHz").textContent =
            sys.cpuMHz + " MHz";

        document.getElementById("flashSize").textContent =
            sys.flashSize;

        document.getElementById("freeHeap").textContent =
            sys.freeHeap;

        document.getElementById("freePsram").textContent =
            sys.freePsram;
    }
    catch(e)
    {
        alert("System read error");
    }
}

//==========================================================
// Clocks
//==========================================================

async function loadClocks()
{
    try
    {
        const response =
            await fetch("/api/clocks");

        const cfg =
            await response.json();

        clocks = cfg;

        drawClocks();
    }
    catch(e)
    {
        alert("Clock read error");
    }
}

//==========================================================

async function loadTimezones()
{
    try
    {
        const response = await fetch("/api/timezones");
        timezones = await response.json();
    }
    catch(e)
    {
        alert("Timezones read error");
    }
}

//==========================================================

function drawClocks()
{
    let html = "";

    for(let i=0;i<clocks.clocks.length;i++)
    {
        const c = clocks.clocks[i];

        let options = "";
        for(let j=0; j<timezones.length; j++)
        {
            const tz = timezones[j];
            const sign = tz.utcHour >= 0 ? "+" : "-";
            const hourStr = Math.abs(tz.utcHour).toString().padStart(2, "0");
            const minStr = tz.utcMinute.toString().padStart(2, "0");
            const offsetStr = `(UTC${sign}${hourStr}:${minStr})`;
            const label = `${tz.nameEN} / ${tz.nameUA} ${offsetStr}`;
            const selected = (c.city === tz.nameEN) ? "selected" : "";
            options += `<option value="${tz.nameEN}" data-index="${tz.index}" ${selected}>${label}</option>`;
        }

        html +=
`
<div class="clockCard">

<div class="clockTitle">
Clock ${i+1}
</div>

<label>City & Timezone</label>

<select id="city${i}">
${options}
</select>

</div>
`;
    }

    document.getElementById("clockContainer").innerHTML = html;
}

//==========================================================

async function saveClocks()
{
    for(let i=0;i<clocks.clocks.length;i++)
    {
        const select = document.getElementById("city"+i);
        const selectedOption = select.options[select.selectedIndex];

        clocks.clocks[i].city = select.value;
        clocks.clocks[i].timezone = parseInt(selectedOption.getAttribute("data-index"));
    }

    try
    {
        const response =
            await fetch(
                "/api/clocks",
                {
                    method:"POST",

                    headers:
                    {
                        "Content-Type":"application/json"
                    },

                    body:JSON.stringify(clocks)
                });

        if(response.ok)
            alert("Clocks saved.");
        else
            alert("Save error.");
    }
    catch(e)
    {
        alert("Communication error.");
    }
}

//==========================================================

window.onload = function()
{
    loadNetwork();
};
