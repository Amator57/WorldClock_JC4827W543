"use strict";

let clocks = [];
let timezones = [];

async function readJson(url)
{
    const response = await fetch(url);

    if (!response.ok)
        throw new Error(`${url}: HTTP ${response.status}`);

    return response.json();
}

async function showPage(pageName, button)
{
    document.querySelectorAll(".page").forEach(p => p.classList.remove("active"));
    document.querySelectorAll(".tabButton").forEach(b => b.classList.remove("active"));
    document.getElementById(pageName).classList.add("active");
    button.classList.add("active");

    switch (pageName)
    {
        case "network":
            await loadNetwork();
            break;

        case "clocks":
            if (timezones.length === 0)
                await loadTimezones();
            await loadClocks();
            break;

        case "system":
            await loadSystem();
            break;
    }
}

function toggleDHCPFields()
{
    const dhcp = document.getElementById("wifi_dhcp").checked;
    document.getElementById("staticIPFields").style.display = dhcp ? "none" : "block";
}

async function loadNetwork()
{
    try
    {
        const cfg = await readJson("/api/network");
        document.getElementById("wifi_ssid").value = cfg.ssid || "";
        document.getElementById("wifi_password").value = cfg.password || "";
        document.getElementById("wifi_dhcp").checked =
            cfg.dhcp !== undefined ? cfg.dhcp : true;
        document.getElementById("wifi_ip").value = cfg.ip || "192.168.1.100";
        document.getElementById("wifi_subnet").value = cfg.subnet || "255.255.255.0";
        document.getElementById("wifi_gateway").value = cfg.gateway || "192.168.1.1";
        document.getElementById("wifi_dns").value = cfg.dns || "8.8.8.8";
        toggleDHCPFields();
    }
    catch (e)
    {
        console.error(e);
        alert("Network read error");
    }
}

async function saveNetwork()
{
    const cfg =
    {
        ssid: document.getElementById("wifi_ssid").value,
        password: document.getElementById("wifi_password").value,
        dhcp: document.getElementById("wifi_dhcp").checked,
        ip: document.getElementById("wifi_ip").value,
        subnet: document.getElementById("wifi_subnet").value,
        gateway: document.getElementById("wifi_gateway").value,
        dns: document.getElementById("wifi_dns").value
    };

    try
    {
        const response = await fetch("/api/network",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(cfg)
        });
        alert(response.ok ? "Network saved." : "Save error.");
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

async function loadSystem()
{
    try
    {
        const sys = await readJson("/api/system");
        document.getElementById("chipModel").textContent = sys.chipModel;
        document.getElementById("chipRevision").textContent = sys.chipRevision;
        document.getElementById("cpuMHz").textContent = `${sys.cpuMHz} MHz`;
        document.getElementById("flashSize").textContent = sys.flashSize;
        document.getElementById("freeHeap").textContent = sys.freeHeap;
        document.getElementById("freePsram").textContent = sys.freePsram;
    }
    catch (e)
    {
        console.error(e);
        alert("System read error");
    }
}

async function loadClocks()
{
    try
    {
        clocks = await readJson("/api/clocks");

        if (!Array.isArray(clocks.clocks))
            throw new Error("Invalid clocks response");

        drawClocks();
    }
    catch (e)
    {
        console.error(e);
        alert("Clock read error");
    }
}

async function loadTimezones()
{
    try
    {
        timezones = await readJson("/api/timezones");
    }
    catch (e)
    {
        console.error(e);
        alert("Timezones read error");
        throw e;
    }
}

function minutesToTime(value)
{
    const minutes = Number(value) || 0;
    const hours = Math.floor(minutes / 60).toString().padStart(2, "0");
    const mins = (minutes % 60).toString().padStart(2, "0");
    return `${hours}:${mins}`;
}

function timeToMinutes(value)
{
    const [hours, minutes] = value.split(":").map(Number);
    return hours * 60 + minutes;
}

function drawClocks()
{
    let html = "";

    for (let i = 0; i < clocks.clocks.length; i++)
    {
        const clock = clocks.clocks[i];
        let options = "";

        for (const tz of timezones)
        {
            const sign = tz.utcHour >= 0 ? "+" : "-";
            const hours = Math.abs(tz.utcHour).toString().padStart(2, "0");
            const minutes = tz.utcMinute.toString().padStart(2, "0");
            const selected = clock.city === tz.nameEN ? " selected" : "";
            const label = `${tz.nameEN} / ${tz.nameUA} (UTC${sign}${hours}:${minutes})`;
            options += `<option value="${tz.nameEN}" data-index="${tz.index}"${selected}>${label}</option>`;
        }

        html += `
<div class="clockCard">
    <div class="clockTitle">Clock ${i + 1}</div>

    <label for="city${i}">City & Timezone</label>
    <select id="city${i}">${options}</select>

    <label>Working hours</label>
    <div class="workTimeRow">
        <input type="time" id="workStart${i}" value="${minutesToTime(clock.workStart)}">
        <span>—</span>
        <input type="time" id="workEnd${i}" value="${minutesToTime(clock.workEnd)}">
    </div>

    <div class="checkboxRow">
        <input type="checkbox" id="workSaturday${i}"${clock.workSaturday ? " checked" : ""}>
        <label for="workSaturday${i}" class="checkboxLabel">Saturday is a workday</label>
    </div>
    <div class="checkboxRow">
        <input type="checkbox" id="workSunday${i}"${clock.workSunday ? " checked" : ""}>
        <label for="workSunday${i}" class="checkboxLabel">Sunday is a workday</label>
    </div>
</div>`;
    }

    document.getElementById("clockContainer").innerHTML = html;
}

async function saveClocks()
{
    for (let i = 0; i < clocks.clocks.length; i++)
    {
        const select = document.getElementById(`city${i}`);
        const option = select.options[select.selectedIndex];
        const clock = clocks.clocks[i];

        clock.city = select.value;
        clock.timezone = Number(option.dataset.index);
        clock.workStart = timeToMinutes(document.getElementById(`workStart${i}`).value);
        clock.workEnd = timeToMinutes(document.getElementById(`workEnd${i}`).value);
        clock.workSaturday = document.getElementById(`workSaturday${i}`).checked;
        clock.workSunday = document.getElementById(`workSunday${i}`).checked;
    }

    try
    {
        const response = await fetch("/api/clocks",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(clocks)
        });
        alert(response.ok ? "Clocks saved." : "Save error.");
    }
    catch (e)
    {
        console.error(e);
        alert("Communication error.");
    }
}

window.onload = () => loadNetwork();
